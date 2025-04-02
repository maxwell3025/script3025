#include <algorithm>
#include <deque>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

namespace parser {
  namespace {
    // From https://stackoverflow.com/questions/29855908/c-unordered-set-of-vectors#answer-29855973
    template <typename T>
    struct VectorHash {
      size_t operator()(const std::vector<T>& v) const {
        std::hash<T> hasher;
        size_t seed = 0;
        for (T i : v) {
          seed ^= hasher(i) + 0x9e3779b9 + (seed<<6) + (seed>>2);
        }
        return seed;
      }
    };

    template <typename T, typename H = std::hash<T>>
    struct UnorderedSetHash {
      size_t operator()(const std::unordered_set<T, H>& set) const {
        H hasher;
        size_t seed = 0;
        for (T element : set) {
          seed ^= hasher(element);
        }
        return seed;
      }
    };

    // A set of production rules for a single nonterminal with `T` as the symbol
    // alphabet.
    // For instance, an instance of `PartialRuleset<char>` would be the
    // production rules for a single character.
    template <typename T>
    using PartialRuleset = std::unordered_set<std::vector<T>, VectorHash<T>>;

    // A full ruleset for a grammar that has `T` as the symbol alphabet.
    // For instance, `TotalRuleset<char>` is the type for grammars on ASCII
    // strings.
    template <typename T>
    using TotalRuleset = std::unordered_map<T, PartialRuleset<T>>;

    template<typename T>
    void vectorize(std::vector<T> &vector) { }

    template<typename T, typename... Args>
    void vectorize(std::vector<T> &vector, T first_element, Args... remaining_elements) {
      vector.push_back(first_element);
      vectorize<T>(vector, remaining_elements...);
    }

    template<typename T, typename... Args>
    std::vector<T> vectorize(Args... elements) {
      std::vector<T> output;
      vectorize<T>(output, elements...);
      return output;
    } // LCOV_EXCL_LINE

    template<typename T>
    struct CanonicalSetEntry {
     public:
      T target;
      const std::vector<T> sentential_form;
      size_t index;
      T following_symbol;

      CanonicalSetEntry(
          T target, const std::vector<T> &sentential_form, T following_symbol)
        : target(target), sentential_form(sentential_form), index(0),
          following_symbol(following_symbol) {}

      CanonicalSetEntry<T> next() const {
        CanonicalSetEntry<T> next = CanonicalSetEntry(*this);
        ++next.index;
        return next;
      }

      bool operator==(const CanonicalSetEntry &rhs) const {
        return target == rhs.target &&
        sentential_form == rhs.sentential_form &&
        index == rhs.index &&
        following_symbol == rhs.following_symbol;
      }
    };

    // Hash function for `CanonicalSetEntry`.
    template <typename T>
    struct CanonicalSetEntryHash {
      size_t operator()(const CanonicalSetEntry<T> &entry) const {
        std::hash<T> t_hasher;
        std::hash<size_t> size_t_hasher;
        VectorHash<T> vec_hasher;

        size_t seed = 0;
        seed ^= t_hasher     (entry.following_symbol)
            + 0x9e3779b9 + (seed<<6) + (seed>>2);
        seed ^= size_t_hasher(entry.index)
            + 0x9e3779b9 + (seed<<6) + (seed>>2);
        seed ^= vec_hasher   (entry.sentential_form)
            + 0x9e3779b9 + (seed<<6) + (seed>>2);
        seed ^= t_hasher     (entry.target)
            + 0x9e3779b9 + (seed<<6) + (seed>>2);
        return seed;
      }
    };
  }

  template <typename T>
  struct Rule {
    T symbol;
    std::vector<T> sentence;
  };

  template <typename T>
  class ConcreteSyntaxTree {
   public:
    T symbol;
    std::vector<std::unique_ptr<ConcreteSyntaxTree<T>>> children;

    ConcreteSyntaxTree(T symbol) : symbol(symbol) {}

    ConcreteSyntaxTree(ConcreteSyntaxTree<T> &&other) : symbol(other.symbol),
        children(std::move(other.children)){}
  };

  template <typename T>
  struct AnnotatedNode {
    std::unique_ptr<ConcreteSyntaxTree<T>> symbol;
    size_t annotation;

    // TODO also try doing this with a non-reference type ans see if it breaks.
    AnnotatedNode(std::unique_ptr<ConcreteSyntaxTree<T>> &&symbol,
                  size_t annotation) : symbol(std::move(symbol)), annotation(annotation) {}
    
    AnnotatedNode(AnnotatedNode &&other) : symbol(std::move(other.symbol)),
                                           annotation(other.annotation) {}
  };

  template <typename T>
  struct ShiftAction {
    size_t resulting_state;
    ShiftAction(size_t resulting_state) : resulting_state(resulting_state) {}
  };

  template <typename T>
  struct ReduceAction {
    size_t pop_qty;
    T result;
    ReduceAction(size_t pop_qty, T result) : pop_qty(pop_qty), result(result) {}
  };

  template <typename T>
  using Action = std::variant<ShiftAction<T>, ReduceAction<T>>;

  template <typename T>
  class ParserBuilder;

  template <typename T>
  class Parser {
    friend class ParserBuilder<T>;

   public:
    Parser(const std::vector<std::unordered_map<T, Action<T>>> &action_table,
           T goal, T end)
      : goal_symbol(goal), terminal_symbol(end), action_table(action_table) { }

    template<typename Iterator>
    ConcreteSyntaxTree<T> parse(Iterator begin, Iterator end){
      while (true) {
        // Log current derivation
        std::cout << " 0 ";
        for (AnnotatedNode<T> &node : stack) {
          std::cout << node.symbol -> symbol << " " << node.annotation << " ";
        }
        std::cout << "        ";
        for (auto it = begin; it != end; ++it) {
          std::cout << *it;
        }
        std::cout << std::endl;

        if (begin == end && stack.size() == 1) {
          // ConcreteSyntaxTree<T> return_value = *stack[0].symbol
          return std::move(*stack[0].symbol);
        }

        T current_lookahead;
        if (begin == end) {
          current_lookahead = terminal_symbol;
        } else {
          current_lookahead = *begin;
        }

        size_t current_state = 0;
        if (!stack.empty()) {
          current_state = stack[stack.size() - 1].annotation;
        }

        if (action_table[current_state].find(current_lookahead) ==
            action_table[current_state].end()) {
          std::cerr << "Error: no action for state " << current_state <<
                       " on lookahead " << current_lookahead << std::endl;
        }

        Action<T> current_action =
            action_table[current_state].at(current_lookahead);
        
        if (std::holds_alternative<ShiftAction<T>>(current_action)) {
          ShiftAction<T> shift_action = std::get<ShiftAction<T>>(current_action);

          std::unique_ptr<ConcreteSyntaxTree<T>> node =
              std::make_unique<ConcreteSyntaxTree<T>>(current_lookahead);

          stack.push_back(AnnotatedNode<T>(std::move(node), shift_action.resulting_state));

          ++begin;
        } else if (std::holds_alternative<ReduceAction<T>>(current_action)) {
          ReduceAction<T> reduce_action = std::get<ReduceAction<T>>(
              current_action);
          
          std::unique_ptr<ConcreteSyntaxTree<T>> node =
              std::make_unique<ConcreteSyntaxTree<T>>(reduce_action.result);

          if (stack.size() < reduce_action.pop_qty) {
            std::cerr << "Error: trying to reduce " << reduce_action.pop_qty <<
                         " from stack of size " << stack.size() << std::endl;
          }

          for (size_t i = 0; i < reduce_action.pop_qty; i++) {
            (node -> children).emplace_back(std::move(stack[stack.size() - reduce_action.pop_qty + i].symbol));
            stack[stack.size() - reduce_action.pop_qty + i].symbol;
          }

          for (size_t i = 0; i < reduce_action.pop_qty; i++) {
            stack.pop_back();
          }

          // Try to shift
          size_t pre_reduce_state = 0;
          if (!stack.empty()) {
            pre_reduce_state = stack.back().annotation;
          }

          if (action_table[pre_reduce_state].find(reduce_action.result) == action_table[pre_reduce_state].end()) {
            std::cerr << "Error: no action on state " <<
                         pre_reduce_state << " with symbol " <<
                         reduce_action.result << std::endl;
          }
          Action<T> action = action_table[pre_reduce_state].at(reduce_action.result);

          if (!std::holds_alternative<ShiftAction<T>>(action)) {
            std::cerr << "Error: non-shift action on state " <<
                         pre_reduce_state << " with symbol " <<
                         reduce_action.result << std::endl;
          }
          ShiftAction<T> shift_action = std::get<ShiftAction<T>>(action);

          stack.push_back(AnnotatedNode<T>(std::move(node),
                                           shift_action.resulting_state));
        }

      }
    }
   private:
    T goal_symbol;
    T terminal_symbol;
    std::vector<std::unordered_map<T, Action<T>>> action_table;
    std::vector<AnnotatedNode<T>> stack;
  };

  template <typename T>
  class ParserBuilder {
    using CanonicalSet = std::unordered_set<CanonicalSetEntry<T>, CanonicalSetEntryHash<T>>;
    using CanonicalSetHash = UnorderedSetHash<CanonicalSetEntry<T>, CanonicalSetEntryHash<T>>;
    using CanonicalCollection = std::unordered_set<CanonicalSet, CanonicalSetHash>;
   public:
    T goal_symbol;
    T terminal_symbol;
    TotalRuleset<T> ruleset;
    std::unordered_set<T> terminals;
    std::unordered_set<T> nonterminals;
    std::unordered_set<T> possibly_empty;
    std::unordered_map<T, std::unordered_set<T>> first_set;
    std::unordered_map<T, std::unordered_set<T>> follow_set;

    ParserBuilder(T goal, T end) : goal_symbol(goal), terminal_symbol(end) {}

    template<typename... Args>
    ParserBuilder<T> &rule(T nonterminal, Args... sentence) {
      std::vector<T> sentence_vector = vectorize<T>(sentence...);
      ruleset[nonterminal].insert(sentence_vector);
      return *this;
    }

    void infer_types() {
      for (std::pair<T, PartialRuleset<T>> symbol_ruleset : ruleset) {
        for (std::vector<T> sentence : symbol_ruleset.second) {
          std::copy(sentence.begin(), sentence.end(),
                    std::inserter(terminals, terminals.end()));
        }
      }

      for (auto it = ruleset.begin(); it != ruleset.end(); ++it) {
        const T &nonterminal = it -> first;
        nonterminals.insert(nonterminal);
        terminals.erase(nonterminal);
      }
    }

    // Returns `true` if `sentence` might produce an empty string according to
    // `possibly_empty`.
    // `possibly_empty` should be a superset of the non-terminals that can
    // actually evaluate to the empty string.
    bool is_sentence_empty(const std::vector<T> &sentence) {
      for (const T &character : sentence) {
        if (possibly_empty.find(character) == possibly_empty.end()) {
          return false;
        }
      }
      return true;
    }
    
    // Returns `true` if `possibly_empty` and the rules for `symbol` guarantee
    // that `symbol` produces a nonempty string.
    // Otherwise, returns `false`.
    bool definitely_nonempty(const T &symbol) {
      bool symbol_has_rule = ruleset.find(symbol) == ruleset.end();
      const PartialRuleset<T> &productions = symbol_has_rule ?
          PartialRuleset<T>() : ruleset.at(symbol);
      
      for (const std::vector<T> &sentence : productions) {
        if (is_sentence_empty(sentence)) {
          return false;
        }
      }

      return true;
    }

    // Determines the set of non-terminals in a set that could produce the empty
    // string.
    std::unordered_set<T> get_empty_set() {
      possibly_empty = nonterminals;
      bool changed = true;
      while (changed) {
        changed = false;
        std::unordered_set<T> nonempty;
        for (const T &symbol : possibly_empty) {
          if (definitely_nonempty(symbol)) {
            changed = true;
            nonempty.insert(symbol);
          }
        }
        for (auto it = nonempty.begin(); it != nonempty.end(); it++) {
          possibly_empty.erase(*it);
        }
      }
      return possibly_empty;
    }

    // Generates the first set for each nonterminal symbol.
    std::unordered_map<T, std::unordered_set<T>> get_first_set() {
      bool changed = true;
      while (changed) {
        changed = false;
        for (const T &symbol : nonterminals) {
          const PartialRuleset<T> &symbol_ruleset = ruleset.at(symbol);
          size_t original_size = first_set[symbol].size();
          for (const std::vector<T> &sentence : symbol_ruleset) {
            if (sentence.size() == 0) {
              continue;
            } else if (nonterminals.find(sentence[0]) == nonterminals.end()) {
              first_set[symbol].insert(sentence[0]);
            } else {

              for (const T &prefix_symbol : sentence) {
                first_set[symbol].merge(first_set[prefix_symbol]);
                if (possibly_empty.find(prefix_symbol) == possibly_empty.end()) {
                  break;
                }
              }
            }
          }
          if (first_set[symbol].size() != original_size) {
            changed = true;
          }
        }
      }
      return first_set;
    }

    // Returns `true` if the follow set was changed.
    bool update_follow_set() {
      bool changed = false;
      for (const std::pair<T, PartialRuleset<T>> &symbol_ruleset : ruleset) {
        for (const std::vector<T> &sentence : symbol_ruleset.second) {
          std::unordered_set<T> prev;
          for (const T &symbol : sentence) {
            for (const T &prev_symbol : prev) {
              if (nonterminals.find(prev_symbol) != nonterminals.end()) {
                size_t original_size = follow_set[prev_symbol].size();
                if (nonterminals.find(symbol) != nonterminals.end()) {
                  follow_set[prev_symbol].merge(
                      std::unordered_set(first_set.at(symbol)));
                } else {
                  follow_set[prev_symbol].insert(symbol);
                }
                if (follow_set[prev_symbol].size() != original_size) {
                  changed = true;
                }
              }
            }
            if (possibly_empty.find(symbol) == possibly_empty.end()) {
              prev.clear();
            }
            if (nonterminals.find(symbol) != nonterminals.end()) {
              prev.insert(symbol);
            }
          }
          for (const T &prev_symbol : prev) {
            size_t original_size = follow_set[prev_symbol].size();
            follow_set[prev_symbol].merge(
                std::unordered_set(follow_set[symbol_ruleset.first])
            );
            if (follow_set[prev_symbol].size() != original_size) {
              changed = true;
            }
          }
        }
      }
      return changed;
    }

    // Generates the follow set for each nonterminal symbol.
    std::unordered_map<T, std::unordered_set<T>> get_follow_set() {
      follow_set[goal_symbol].insert(terminal_symbol);
      while (update_follow_set());
      return follow_set;
    }

    // Expands a canonical set using the curent rules.
    void extend(CanonicalSet &canonical_set) const {
      size_t prev_size = -1;
      while (canonical_set.size() != prev_size) {
        prev_size = canonical_set.size();
        CanonicalSet new_entries;
        for (const CanonicalSetEntry<T> &entry : canonical_set) {
          // We expand with the canonical entries for starting nonterminals.
          // We can't expand if there is no "next" symbol.
          if (entry.index >= entry.sentential_form.size()) continue;

          T current_symbol = entry.sentential_form[entry.index];
          // We can't expand terminals
          if (nonterminals.find(current_symbol) == nonterminals.end()) continue;

          // Go through the following symbol and merge in first symbols until
          // we hit a nonempty.
          std::unordered_set<T> possible_follows;
          size_t following_symbol_index = entry.index + 1;
          for (; following_symbol_index < entry.sentential_form.size();
              ++following_symbol_index) {
            T following_symbol = entry.sentential_form[following_symbol_index];

            // We merge all of the first symbols for the current following symbol
            // into the possible follows.
            if (nonterminals.find(following_symbol) == nonterminals.end()) {
              possible_follows.insert(following_symbol);
            } else {
              possible_follows.merge(std::unordered_set<T>(first_set.at(following_symbol)));
            }
            
            if (possibly_empty.find(following_symbol) ==
                possibly_empty.end()) {
              break;
            }
          }

          if (following_symbol_index == entry.sentential_form.size()) {
            possible_follows.insert(entry.following_symbol);
          }

          for (T follow_symbol : possible_follows) {
            for (const std::vector<T> &sentential_form
                : ruleset.at(current_symbol)) {
              new_entries.insert(CanonicalSetEntry<T>(
                current_symbol,
                sentential_form,
                follow_symbol
              ));
            }
          }
        }
        canonical_set.merge(CanonicalSet(new_entries));
      }
    }

    CanonicalSet shift(const CanonicalSet &canonical_set, T lookahead) const {
      CanonicalSet shifted_set;
      for (const CanonicalSetEntry<T> &current_entry : canonical_set) {
        if (current_entry.index == current_entry.sentential_form.size())
            continue;
        if (current_entry.sentential_form[current_entry.index] != lookahead)
            continue;
        shifted_set.insert(current_entry.next());
      }
      extend(shifted_set);
      return shifted_set;
    }

    std::string stringify(const CanonicalSet &canonical_set) const {
      std::stringstream output;
      for (const CanonicalSetEntry<T> &entry : canonical_set) {
        output << entry.target;
        output << " <- ";
        for (size_t i = 0; i < entry.sentential_form.size(); ++i) {
          if (i == entry.index) output << "·";
          output << entry.sentential_form[i];
        }
        if (entry.sentential_form.size() == entry.index) output << "·";
        output << ",";
        output << entry.following_symbol;
        output << std::endl;
      }
      return output.str();
    }

    Parser<T> build() {
      infer_types();
      get_empty_set();
      get_first_set();

      std::unordered_set<T> symbols = {terminal_symbol};
      symbols.merge(std::unordered_set<T>(terminals));
      symbols.merge(std::unordered_set<T>(nonterminals));

      CanonicalCollection canonical_collection;

      std::deque<CanonicalSet> bfs_queue;

      // On the set S, and seeing a lookahead L, do action action_table[S, L].
      std::unordered_map<
        CanonicalSet,
        std::unordered_map<T, Action<T>>,
        CanonicalSetHash> action_table;

      CanonicalSet initial_set;
      initial_set.insert(CanonicalSetEntry<T>(goal_symbol,
                                              std::vector<T>{goal_symbol},
                                              terminal_symbol));

      std::vector<CanonicalSet> numbering;

      extend(initial_set);

      bfs_queue.push_back(initial_set);
      while (!bfs_queue.empty()) {
        CanonicalSet current_set = bfs_queue.front();
        bfs_queue.pop_front();

        if (canonical_collection.find(current_set) !=
            canonical_collection.end()) continue;

        canonical_collection.insert(current_set);

        std::cout << "Processing canonical set " << numbering.size() << ":" << std::endl;
        std::cout << stringify(current_set) << std::endl;

        numbering.push_back(current_set);
        
        // Figure out how to handle the lookahead for each canonical set.
        // We defer construction of shifts until after the numbering is stable
        for (T symbol : symbols) {
          bool found_action = false;
          // We look through all entries and check if we can reduce them with
          // the current symbol.
          for (const CanonicalSetEntry<T> &entry : current_set) {
            // If we can reduce the current symbol.
            if (entry.index == entry.sentential_form.size() &&
                entry.following_symbol == symbol) {
              if (found_action) {
                std::cout << "Error: Reduce-Reduce conflict with \'" << symbol << "\' on" << std::endl;
                std::cout << stringify(current_set) << std::endl;
              }
              found_action = true;
              action_table[current_set].insert({symbol, ReduceAction<T>(entry.sentential_form.size(), entry.target)});
            }
          }
          CanonicalSet shifted = shift(current_set, symbol);
          if (!shifted.empty()) {
            if (found_action) {
              std::cout << "Error: Shift-Reduce conflict with \'" << symbol << "\' on" << std::endl;
              std::cout << stringify(current_set) << std::endl;
            }
            bfs_queue.push_back(shifted);
          }
        }
      }

      // TODO: Populate the action table with the shift actions.
      for (size_t canonical_set_index = 0;
           canonical_set_index < numbering.size(); ++canonical_set_index) {
        CanonicalSet canonical_set = numbering[canonical_set_index];
        for (T symbol : symbols) {
          CanonicalSet shifted_set = shift(canonical_set, symbol);
          if (!shifted_set.empty()) {
            size_t new_index = 0;
            while(numbering[new_index] != shifted_set) ++new_index;
            action_table[canonical_set].insert({symbol, Action<T>(ShiftAction<T>(new_index))});
          }
        }
      }

      std::vector<std::unordered_map<T, Action<T>>> action_table_vector;
      for (CanonicalSet canonical_set : numbering) {
        action_table_vector.push_back(action_table[canonical_set]);
      }
      
      return Parser<T>(action_table_vector, goal_symbol, terminal_symbol);
    }

   private:
  };
}