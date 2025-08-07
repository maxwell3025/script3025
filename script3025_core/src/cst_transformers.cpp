#include "cst_transformers.hpp"

namespace script3025 {

void collect_lists(parser::ConcreteSyntaxTree<Token> &tree) {
  for (parser::ConcreteSyntaxTree<Token> &child : tree.children) {
    collect_lists(child);
  }

  if (tree.symbol == Token::PROG && tree.children.size() == 2) {
    if (tree.children[0].symbol == Token::PROG &&
        tree.children[1].symbol == Token::DEFN) {
      parser::ConcreteSyntaxTree<Token> definition =
          std::move(tree.children[1]);
      tree.children = std::move(tree.children[0].children);
      tree.children.emplace_back(std::move(definition));
    }
  }
}

[[nodiscard]] static bool is_expr(const Token &t) {
  switch (t) {
    case Token::EXPR_EQ:
    case Token::EXPR_ABS:
    case Token::EXPR_APP:
    case Token::EXPR_PAREN:
    case Token::ABS:
      return true;
    default:
      return false;
  }
}

void collapse_oop(parser::ConcreteSyntaxTree<Token> &tree) {
  for (parser::ConcreteSyntaxTree<Token> &child : tree.children) {
    collapse_oop(child);
  }

  if (is_expr(tree.symbol)) {
    tree.symbol = Token::EXPR;
    if (tree.children.size() == 1 && is_expr(tree.children[0].symbol))
      tree.children = std::move(tree.children[0].children);
  }
}

}  // namespace script3025
