#ifndef SCRIPT3025_SCRIPT3025_CORE_PI_EXPRESSION_HPP
#define SCRIPT3025_SCRIPT3025_CORE_PI_EXPRESSION_HPP

#include "expression/expression_base.hpp"
#include "expression/expression_visitor.hpp"

namespace script3025 {

class PiExpression : public Expression {
 public:
  PiExpression(std::string argument_id,
               std::unique_ptr<Expression>&& argument_type,
               std::unique_ptr<Expression>&& definition);
  PiExpression();
  void accept(ExpressionVisitor& visitor) const override;
  void accept(MutatingExpressionVisitor& visitor) override;
  inline std::unique_ptr<Expression>& argument_type() { return children[0]; }
  inline std::unique_ptr<Expression>& definition() { return children[1]; }
  inline const std::unique_ptr<Expression>& argument_type() const {
    return children[0];
  }
  inline const std::unique_ptr<Expression>& definition() const { return children[1]; }

  std::string argument_id;

 private:
  static std::shared_ptr<spdlog::logger> get_logger();
};

}  // namespace script3025

#endif
