#include "expression/subtypes/let_expression.hpp"

#include <memory>
#include <string>
#include <utility>

#include "expression/expression_base.hpp"
#include "expression/subtypes/scope_expression.hpp"

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

}  // namespace script3025
