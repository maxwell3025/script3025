#ifndef SCRIPT3025_SCRIPT3025_CORE_DEFINITION_HPP
#define SCRIPT3025_SCRIPT3025_CORE_DEFINITION_HPP

#include <memory>
#include <string>

#include "expression/expression.hpp"
#include "expression_factory.hpp"
#include "parser.hpp"

namespace script3025 {

class Definition {
 public:
  template <typename Iterator>
  Definition(parser::ConcreteSyntaxTree<Token> &source, Iterator &&string_iterator);
  std::string name;
  std::unique_ptr<Expression> definition;
 private:
  static std::shared_ptr<spdlog::logger> get_logger();
};

template <typename Iterator>
Definition::Definition(parser::ConcreteSyntaxTree<Token> &source,
                       Iterator &&string_iterator) {
  if (source.symbol != Token::DEFN) {
    SPDLOG_LOGGER_ERROR(get_logger(), "Expected a concrete syntax tree with type {}. Received {}",
                        Token::PROG, source.symbol);
    throw std::runtime_error(
        "Attempted to construct program from non-program node.");
  }

  std::vector<Token> sentential_form;
  std::transform(source.children.begin(), source.children.end(),
                 std::back_inserter(sentential_form),
                 [](const parser::ConcreteSyntaxTree<Token>& child) -> Token {
                   return child.symbol;
                 });

  if(sentential_form != std::vector({Token::DEF, Token::ID, Token::ASSIGN,
                                     Token::EXPR})) {
      SPDLOG_LOGGER_ERROR(get_logger(), "Received malformed syntax tree:\n"
                       "{}", source.to_string());
  }

  // def
  ++string_iterator;
  // <identifier>
  name = *string_iterator;
  ++string_iterator;
  // :=
  ++string_iterator;
  // <expr>
  definition = create_expression(source.children[3], string_iterator);
}

}

#endif