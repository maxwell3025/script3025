#ifndef SCRIPT3025_SCRIPT3025_CORE_PROGRAM_HPP
#define SCRIPT3025_SCRIPT3025_CORE_PROGRAM_HPP

#include <vector>

#include "parser.hpp"

#include "definition.hpp"
#include "expression/expression.hpp"
#include "inductive_definition.hpp"

namespace script3025 {

class Program {
 public:
  template <typename Iterator>
  Program(parser::ConcreteSyntaxTree<Token> &source, Iterator &&string_iterator);
  std::vector<Definition> definitions;
  std::vector<InductiveDefinition> inductive_definitions;
 private:
  static std::shared_ptr<spdlog::logger> get_logger();
};

template <typename Iterator>
Program::Program(parser::ConcreteSyntaxTree<Token> &source,
                 Iterator &&string_iterator) {
  if (source.symbol != Token::PROG) {
    SPDLOG_LOGGER_ERROR(get_logger(), "Expected a concrete syntax tree with type {}. Received {}",
                     Token::PROG, source.symbol);
    throw std::runtime_error(
        "Attempted to construct program from non-program node.");
  }
  for (parser::ConcreteSyntaxTree<Token> &child : source.children) {
    switch (child.symbol) {
    case Token::DEFN:
      definitions.emplace_back(child, string_iterator);
      break;
    case Token::IDEFN:
      inductive_definitions.emplace_back(child, string_iterator);
      break;
    default:
      SPDLOG_LOGGER_ERROR(get_logger(), "Received malformed syntax tree:\n"
                          "{}", source);
      throw std::runtime_error("Malformed syntax tree");
    }
  }
}

} // namespace script3025

#endif