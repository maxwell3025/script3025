#include "expression/subtypes/let_expression.hpp"

namespace script3025 {

LetExpression::LetExpression(std::string argument_id,
                             std::unique_ptr<Expression> &&argument_type,
                             std::unique_ptr<Expression> &&argument_value,
                             std::unique_ptr<Expression> &&definition)
    : ScopeExpression(std::move(argument_id)) {
  children.emplace_back(std::move(argument_type));
  children.emplace_back(std::move(argument_value));
  children.emplace_back(std::move(definition));
}

LetExpression::LetExpression() {}

void LetExpression::accept(ExpressionVisitor &visitor) const {
  visitor.visit_let(*this);
}

void LetExpression::accept(MutatingExpressionVisitor &visitor) {
  visitor.visit_let(*this);
}

}  // namespace script3025
