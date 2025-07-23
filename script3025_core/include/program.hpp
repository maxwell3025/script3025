#ifndef SCRIPT3025_SCRIPT3025_CORE_PROGRAM_HPP
#define SCRIPT3025_SCRIPT3025_CORE_PROGRAM_HPP

#include <vector>

#include "parser.hpp"
#include "spdlog/fmt/fmt.h"
#include "spdlog/fmt/ranges.h"

#include "expression/expression.hpp"
#include "expression_factory.hpp"

namespace script3025 {

class Program {
 public:
  Program(std::string source);

  constexpr const std::unordered_map<std::string, std::unique_ptr<Expression>> &
  global_definitions() const {
    return global_definitions_;
  }

  constexpr const std::vector<std::string> &global_names() const {
    return global_names_;
  }

  inline const Expression &global(std::string id) const {
    return *global_definitions_.at(id);
  }

  inline const bool comes_before(std::string a, std::string b) const {
    if (ordering_.find(a) == ordering_.end()) return false;
    if (ordering_.find(b) == ordering_.end()) return false;
    return ordering_.at(a) < ordering_.at(b);
  }

  inline void push_definition(std::string name,
                              std::unique_ptr<Expression> definition) {
    global_definitions_.emplace(name, std::move(definition));
    ordering_.emplace(name, global_names_.size());
    global_names_.push_back(name);
  }

  inline void pop_definition() {
    global_definitions_.erase(global_names_.back());
    ordering_.erase(global_names_.back());
    global_names_.pop_back();
  }

  std::string to_string() const;

  std::unique_ptr<Expression> get_type(const std::string &target);

  std::unique_ptr<Expression> get_reduced(const std::string &target);

 private:
  template <typename Iterator>
  void add_program(parser::ConcreteSyntaxTree<Token> &source,
                   Iterator &string_iterator) {
    if (source.symbol != Token::PROG) {
      SPDLOG_LOGGER_ERROR(
          get_logger(),
          "Expected a concrete syntax tree with type {}. Received {}",
          Token::PROG, source.symbol);
      throw std::runtime_error(
          "Attempted to construct program from non-program node.");
    }
    for (parser::ConcreteSyntaxTree<Token> &child : source.children) {
      if (child.symbol != Token::DEFN) {
        SPDLOG_LOGGER_ERROR(get_logger(),
                            "Received malformed syntax tree:\n"
                            "{}",
                            source);
        continue;
      }
      add_definition(child, string_iterator);
    }
  }

  template <typename Iterator>
  void add_definition(parser::ConcreteSyntaxTree<Token> &source,
                      Iterator &string_iterator) {
    if (source.symbol != Token::DEFN) {
      SPDLOG_LOGGER_ERROR(
          get_logger(),
          "Expected a concrete syntax tree with type {}. Received {}",
          Token::PROG, source.symbol);
      return;
    }

    static const std::vector<Token> defn_sentence{Token::DEF, Token::ID,
                                                  Token::ASSIGN, Token::EXPR};

    if (source.sentence() != defn_sentence) {
      SPDLOG_LOGGER_ERROR(
          get_logger(),
          "Expected a concrete syntax tree with form {}. Received {}",
          defn_sentence, source.sentence());
      return;
    }

    ++string_iterator;
    std::string id = *string_iterator;
    ++string_iterator;
    ++string_iterator;
    push_definition(id, create_expression(source.children[3], string_iterator));
  }

  static std::shared_ptr<spdlog::logger> get_logger();

  std::unordered_map<std::string, size_t> ordering_;
  std::unordered_map<std::string, std::unique_ptr<Expression>>
      global_definitions_;
  std::vector<std::string> global_names_;
};

inline std::ostream &operator<<(std::ostream &os, const script3025::Program &prog) {
  return os << prog.to_string();
}

}  // namespace script3025

template <>
struct fmt::formatter<script3025::Program> {
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }

  format_context::iterator format(const script3025::Program &prog,
                                  format_context &ctx) const {
    return fmt::format_to(ctx.out(), "{0}", prog.to_string());
  };
};

#endif