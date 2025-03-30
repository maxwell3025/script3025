#include <algorithm>
#include <memory>
#include <unordered_map>
#include <unordered_set>
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
  }

  template <typename T>
  struct Rule {
    T symbol;
    std::vector<T> sentence;
  };

  template <typename T>
  class ConcreteSyntaxtree {
    T symbol;
    std::unique_ptr<ConcreteSyntaxtree> parent;
    std::vector<std::unique_ptr<ConcreteSyntaxtree>> children;
  };

  template <typename T>
  class ParserBuilder {
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
      for (auto it = ruleset.begin(); it != ruleset.end(); ++it) {
        PartialRuleset<T> &symbol_ruleset = it -> second;
        for (auto it2 = symbol_ruleset.begin(); it2 != symbol_ruleset.end(); ++it2) {
          std::copy(it2 -> begin(), it2 -> end(),
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
      for (auto it = sentence.begin(); it != sentence.end(); it++) {
        if (possibly_empty.find(*it) == possibly_empty.end()) {
          return false;
        }
      }
      return true;
    }
    
    // Returns `true` if `possibly_empty` and the rules for `symbol` guarantee
    // that `symbol` produces a nonempty string.
    // Otherwise, returns `false`.
    bool definitely_nonempty(const T &symbol) {
      const PartialRuleset<T> &productions = ruleset.find(symbol) == ruleset.end() ?
      PartialRuleset<T>() : ruleset.at(symbol);
      
      for (auto it = productions.begin(); it != productions.end(); it++) {
        if (is_sentence_empty(*it)) {
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
        for (auto it = possibly_empty.begin(); it != possibly_empty.end(); it++) {
          if (definitely_nonempty(*it)) {
            changed = true;
            nonempty.insert(*it);
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
        for (auto it = nonterminals.begin(); it != nonterminals.end(); ++it) {
          const T &symbol = *it;
          const PartialRuleset<T> &symbol_ruleset = ruleset.at(symbol);
          size_t original_size = first_set[symbol].size();
          for (auto it2 = symbol_ruleset.begin(); it2 != symbol_ruleset.end();
              ++it2) {
            std::vector<T> sentence = *it2;
            if (sentence.size() == 0) {
              continue;
            } else if (nonterminals.find(sentence[0]) == nonterminals.end()) {
              first_set[symbol].insert(sentence[0]);
            } else {
              for (auto it3 = sentence.begin(); it3 != sentence.end(); ++it) {
                first_set[symbol].merge(first_set[*it3]);
                if (possibly_empty.find(*it3) == possibly_empty.end()) {
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
      for (auto it = ruleset.begin(); it != ruleset.end(); ++it) {
        const PartialRuleset<T> &symbol_ruleset = it -> second;
        for (auto it2 = symbol_ruleset.begin(); it2 != symbol_ruleset.end(); ++it2) {
          const std::vector<T> &sentence = *it2;
          std::unordered_set<T> prev;
          for (auto it3 = sentence.begin(); it3 != sentence.end(); ++it3) {
            const T &symbol = *it3;
            for (auto it4 = prev.begin(); it4 != prev.end(); ++it4) {
              const T &prev_symbol = *it4;
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
          for (auto it3 = prev.begin(); it3 != prev.end(); ++it3) {
            size_t original_size = follow_set[*it3].size();
            follow_set[*it3].merge(std::unordered_set(follow_set[it -> first]));
            if (follow_set[*it3].size() != original_size) {
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
   private:
  };

  template <typename T>
  class Parser {
    using Sentence = std::vector<T>;
  public:
    Parser(std::vector<Rule<T>> rules) {
        std::unordered_set<T> terminals;
        std::unordered_set<T> nonterminals;
        std::unordered_map<T, std::unordered_set<Sentence>> ruleset;

        for (auto it = rules.begin(); it != rules.end(); it++) {
            Rule<T> &rule = *it;
            std::vector<T> &sentence = rule.sentence;
            for (auto it2 = sentence.begin(); it2 != sentence.end(); it2++) {
                T &sentence_symbol = *it2;
                terminals.insert(sentence_symbol);
            }
        }

        for(auto it = rules.begin(); it != rules.end(); it++) {
            Rule<T> &rule = *it;
            terminals.erase(rule.symbol);
            nonterminals.insert(rule.symbol);
            ruleset[rule.symbol].insert(rule.sentence);
        }

        std::unordered_set<T> get_empty_set = get_empty_set(
          nonterminals,
          ruleset
        );

        std::unordered_map<T, std::unordered_set<T>> first_set;
    }
  };
}