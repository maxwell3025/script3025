#include "let_expression.hpp"

namespace script3025 {

LetExpression::LetExpression(std::string &&argument_id,
                             std::unique_ptr<Expression> &&argument_type,
                             std::unique_ptr<Expression> &&argument_value,
                             std::unique_ptr<Expression> &&definition)
                             : argument_id(argument_id),
                             argument_type(std::move(argument_type)),
                             definition(std::move(definition)) {}

LetExpression::LetExpression(const LetExpression &other)
    : argument_value(other.argument_value -> clone()),
    argument_id(other.argument_id),
    argument_type(other.argument_type -> clone()),
    definition(other.definition -> clone()) {}

LetExpression::LetExpression() {}

bool LetExpression::is_normal() const {
  return false;
}

void LetExpression::accept(ExpressionVisitor &visitor) const {
  visitor.visit_let(*this);
}

void LetExpression::accept(MutatingExpressionVisitor &visitor) {
  visitor.visit_let(*this);
}

}
