#ifndef SCRIPT3025_SCRIPT3025_CORE_SCRIPT3025_CORE_HPP
#define SCRIPT3025_SCRIPT3025_CORE_SCRIPT3025_CORE_HPP

#include <string>
#include <unordered_map>

#include "parser.hpp"

#include "expression.hpp"
#include "parsing_utility.hpp"

namespace script3025 {

class InductiveDefinition {
 public:
  template <typename Iterator>
  InductiveDefinition(parser::ConcreteSyntaxTree<Token> &source, Iterator &string_iterator);
 private:
  std::string name;
  std::unique_ptr<Expression> definition;
};

class NormalDefinition {
 public:
  template <typename Iterator>
  NormalDefinition(parser::ConcreteSyntaxTree<Token> &source, Iterator &string_iterator);
 private:
  std::string name;
  std::unique_ptr<Expression> definition;
};

class Program {
 public:
  template <typename Iterator>
  Program(parser::ConcreteSyntaxTree<Token> &source, Iterator &string_iterator);
 private:
  std::vector<NormalDefinition> definitions;
  std::vector<InductiveDefinition> inductive_definitions;
};

void collect_lists(parser::ConcreteSyntaxTree<Token> &tree);

void collapse_oop(parser::ConcreteSyntaxTree<Token> &tree);

template <typename Iterator>
NormalDefinition::NormalDefinition(parser::ConcreteSyntaxTree<Token> &source,
                                   Iterator &string_iterator) {
  if (source.symbol != Token::DEFN) {
    LOGGER3025_ERROR("Expected a concrete syntax tree with type {}. Received {}",
                     to_string(Token::PROG), to_string(source.symbol));
    throw std::runtime_error(
        "Attempted to construct program from non-program node.");
  }
  for (parser::ConcreteSyntaxTree<Token> child : source.children) {
    switch (child.symbol) {
    case Token::ID:
      name = *string_iterator;
      ++string_iterator;
      break;
    case Token::EXPR:
      definition = (child, string_iterator);
      break;
    default:
      LOGGER3025_ERROR("Received malformed syntax tree:\n"
                       "{}", to_string(source));
      throw std::runtime_error("Malformed syntax tree");
    }
  }
}

template <typename Iterator>
Program::Program(parser::ConcreteSyntaxTree<Token> &source,
                 Iterator &string_iterator) {
  if (source.symbol != Token::PROG) {
    LOGGER3025_ERROR("Expected a concrete syntax tree with type {}. Received {}",
                     to_string(Token::PROG), to_string(source.symbol));
    throw std::runtime_error(
        "Attempted to construct program from non-program node.");
  }
  for (parser::ConcreteSyntaxTree<Token> child : source.children) {
    switch (child.symbol) {
    case Token::DEFN:
      definitions.emplace_back(child, string_iterator);
      break;
    case Token::IDEFN:
      inductive_definitions.emplace_back(child, string_iterator);
      break;
    default:
      LOGGER3025_ERROR("Received malformed syntax tree:\n"
                       "{}", to_string(source));
      throw std::runtime_error("Malformed syntax tree");
    }
  }
}

} // namespace script3025

#endif
