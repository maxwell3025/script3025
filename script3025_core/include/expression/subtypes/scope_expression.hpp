#ifndef SCRIPT3025_SCRIPT3025_CORE_EXPRESSION_SUBTYPES_SCOPE_EXPRESSION_HPP
#define SCRIPT3025_SCRIPT3025_CORE_EXPRESSION_SUBTYPES_SCOPE_EXPRESSION_HPP

#include <memory>
#include <string>
#include <utility>

#include "expression/expression_base.hpp"
#include "expression/expression_visitor.hpp"

namespace script3025 {

class ScopeExpression : public Expression {
 public:
  [[nodiscard]] virtual std::unique_ptr<Expression>& argument_type() = 0;
  [[nodiscard]] virtual std::unique_ptr<Expression>& definition() = 0;
  [[nodiscard]] virtual const std::unique_ptr<Expression>& argument_type()
      const = 0;
  [[nodiscard]] virtual const std::unique_ptr<Expression>& definition()
      const = 0;

  std::string argument_id;

 protected:
  ScopeExpression(std::string argument_id)
      : argument_id(std::move(argument_id)) {}
  ScopeExpression() = default;
};

}  // namespace script3025

#endif
