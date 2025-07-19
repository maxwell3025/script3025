#include "expression/subtypes/let_expression.hpp"

namespace script3025 {

LetExpression::LetExpression(std::string argument_id,
                             std::unique_ptr<Expression> &&argument_type,
                             std::unique_ptr<Expression> &&argument_value,
                             std::unique_ptr<Expression> &&definition)
                             : argument_id(argument_id),
                             Expression{std::move(argument_type),
                                        std::move(argument_value),
                                        std::move(definition)} {}

LetExpression::LetExpression() {}

void LetExpression::accept(ExpressionVisitor &visitor) const {
  visitor.visit_let(*this);
}

void LetExpression::accept(MutatingExpressionVisitor &visitor) {
  visitor.visit_let(*this);
}

}
