#include "let_expression.hpp"

namespace script3025 {

void LetExpression::accept(ExpressionVisitor &visitor) const {
  visitor.visit_let(*this);
}

void LetExpression::accept(MutatingExpressionVisitor &visitor) {
  visitor.visit_let(*this);
}

}
