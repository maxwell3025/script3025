#ifndef SCRIPT3025_SCRIPT3025_CORE_PROGRAM_HPP
#define SCRIPT3025_SCRIPT3025_CORE_PROGRAM_HPP

#include <vector>

#include "spdlog/fmt/ranges.h"
#include "parser.hpp"

#include "expression/expression.hpp"
#include "expression_factory.hpp"

namespace script3025 {

class Program {
 public:
  template <typename Iterator>
  Program(parser::ConcreteSyntaxTree<Token> &source, Iterator &string_iterator);

  template <typename Iterator>
  void add_definition(parser::ConcreteSyntaxTree<Token> &source,
                      Iterator &string_iterator);

  std::unordered_map<std::string, std::unique_ptr<Expression>> definitions;
  std::vector<std::string> definition_order;

 private:
  static std::shared_ptr<spdlog::logger> get_logger();
};

template <typename Iterator>
Program::Program(parser::ConcreteSyntaxTree<Token> &source,
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
void Program::add_definition(parser::ConcreteSyntaxTree<Token> &source,
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
  definition_order.push_back(id);
  ++string_iterator;
  ++string_iterator;
  definitions[id] = create_expression(source.children[3], string_iterator);
}

}  // namespace script3025

#endif