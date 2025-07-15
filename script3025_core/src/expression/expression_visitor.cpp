#include "expression/expression_visitor.hpp"

#include "expression/expression_base.hpp"

namespace script3025 {

void ExpressionVisitor::visit(const Expression &e) {
  e.accept(*this);
}

void MutatingExpressionVisitor::visit(Expression &e) {
  e.accept(*this);
}

} // namespace script3025
