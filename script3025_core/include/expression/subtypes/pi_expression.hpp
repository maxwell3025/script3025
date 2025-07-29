#ifndef SCRIPT3025_SCRIPT3025_CORE_PI_EXPRESSION_HPP
#define SCRIPT3025_SCRIPT3025_CORE_PI_EXPRESSION_HPP

#include "expression/expression_visitor.hpp"
#include "scope_expression.hpp"

namespace script3025 {

class PiExpression : public ScopeExpression {
 public:
  PiExpression(std::string argument_id,
               std::unique_ptr<Expression>&& argument_type,
               std::unique_ptr<Expression>&& definition);
  PiExpression();
  void accept(ExpressionVisitor<true> &visitor) const override { visitor.visit_pi(*this); }
  void accept(ExpressionVisitor<false> &visitor) override { visitor.visit_pi(*this); }
  std::unique_ptr<Expression>& argument_type() override { return children[0]; }
  std::unique_ptr<Expression>& definition() override { return children[1]; }
  const std::unique_ptr<Expression>& argument_type() const override {
    return children[0];
  }
  const std::unique_ptr<Expression>& definition() const override {
    return children[1];
  }

  std::string argument_id;

 private:
  static std::shared_ptr<spdlog::logger> get_logger();
};

}  // namespace script3025

#endif
