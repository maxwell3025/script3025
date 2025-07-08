#ifndef SCRIPT3025_SCRIPT3025_CORE_PI_EXPRESSION_HPP
#define SCRIPT3025_SCRIPT3025_CORE_PI_EXPRESSION_HPP

#include "expression.hpp"
#include "expression_visitor.hpp"

namespace script3025 {

class PiExpression : public Expression {
 public:
  PiExpression(std::string &&argument_id,
               std::unique_ptr<Expression> &&argument_type,
               std::unique_ptr<Expression> &&definition);
  PiExpression(const PiExpression &other);
  PiExpression();
  bool is_normal() const override;
  std::ostream &print(std::ostream &os) const override;
  void accept(ExpressionVisitor &visitor) const override;
  void accept(MutatingExpressionVisitor &visitor) override;

  std::string argument_id;
  std::unique_ptr<Expression> argument_type;
  std::unique_ptr<Expression> definition;

 private:
  static std::shared_ptr<spdlog::logger> get_logger();
};

} // namespace script3025

#endif
