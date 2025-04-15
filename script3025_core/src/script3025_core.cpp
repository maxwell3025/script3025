#include "script3025_core.hpp"

#include "logger3025.hpp"

namespace script3025 {

// @brief Collect all of the elements of list-like symbols.
//
// For instance, we want
// ```
// ┬prog
// ├┬prog
// │├┬prog
// ││├┬prog
// │││├─prog
// │││└─defn
// ││└─defn
// │└─defn
// └─defn
// ```
// To collapse into
// ```
// ┬prog
// ├─defn
// ├─defn
// ├─defn
// └─defn
// ```
void collect_lists(parser::ConcreteSyntaxTree<Token> &tree) {
  for (parser::ConcreteSyntaxTree<Token> &child : tree.children) {
    collect_lists(child);
  }

  if (tree.symbol == Token::PROG && tree.children.size() == 2) {
    if (tree.children[0].symbol == Token::PROG &&
       tree.children[1].symbol == Token::DEFN){
      parser::ConcreteSyntaxTree<Token> definition = std::move(tree.children[1]);
      tree.children = std::move(tree.children[0].children);
      tree.children.emplace_back(std::move(definition));
    }
  }
}

// @brief Collapse the order-of-operations rules in the CST
void collapse_oop(parser::ConcreteSyntaxTree<Token> &tree) {
  for (parser::ConcreteSyntaxTree<Token> &child : tree.children) {
    collapse_oop(child);
  }

  switch (tree.symbol) {
  case Token::EXPR_ABS:
  case Token::EXPR_APP:
    if (tree.children.size() == 1) {
      tree.children = std::move(tree.children[0].children);
    }
    tree.symbol = Token::EXPR;
  case Token::EXPR_PAREN:
    tree.symbol = Token::EXPR;
    break;
  default:
    break;
  }
}

Environment::Environment(parser::ConcreteSyntaxTree<Token> &source) {
}

} // namespace script3025
