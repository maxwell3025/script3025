#ifndef SCRIPT3025_SCRIPT3025_CORE_LAMBDA_EXPRESSION_HPP
#define SCRIPT3025_SCRIPT3025_CORE_LAMBDA_EXPRESSION_HPP

#include "expression/expression_base.hpp"
#include "expression/expression_visitor.hpp"

namespace script3025 {

class LambdaExpression : public Expression {
 public:
  LambdaExpression(std::string &&argument_id,
                   std::unique_ptr<Expression> &&argument_type,
                   std::unique_ptr<Expression> &&definition);
  LambdaExpression(const LambdaExpression &other);
  LambdaExpression();
  void accept(ExpressionVisitor &visitor) const override;
  void accept(MutatingExpressionVisitor &visitor) override;
  virtual std::vector<Expression *> get_children() const override;

  std::string argument_id;
  std::unique_ptr<Expression> argument_type;
  std::unique_ptr<Expression> definition;

 private:
  static std::shared_ptr<spdlog::logger> get_logger();
};

} // namespace script3025

#endif
