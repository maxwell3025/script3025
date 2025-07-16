#ifndef SCRIPT3025_SCRIPT3025_CORE_EQUALITY_EXPRESSION_HPP
#define SCRIPT3025_SCRIPT3025_CORE_EQUALITY_EXPRESSION_HPP

#include "expression/expression_base.hpp"
#include "expression/expression_visitor.hpp"

namespace script3025 {

class EqualityExpression : public Expression {
 public:
  EqualityExpression(std::unique_ptr<Expression> &&lhs,
                     std::unique_ptr<Expression> &&rhs);
  EqualityExpression(const EqualityExpression &other);
  EqualityExpression();
  void accept(ExpressionVisitor &visitor) const override;
  void accept(MutatingExpressionVisitor &visitor) override;

  std::unique_ptr<Expression> lhs;
  std::unique_ptr<Expression> rhs;

 private:
  static std::shared_ptr<spdlog::logger> get_logger();
};

} // namespace script3025

#endif
