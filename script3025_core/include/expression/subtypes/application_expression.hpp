#ifndef SCRIPT3025_SCRIPT3025_CORE_APPLICATION_EXPRESSION_HPP
#define SCRIPT3025_SCRIPT3025_CORE_APPLICATION_EXPRESSION_HPP

#include "expression/expression_base.hpp"
#include "expression/expression_visitor.hpp"

namespace script3025 {

class ApplicationExpression : public Expression {
 public:
  ApplicationExpression(std::unique_ptr<Expression> &&function,
                        std::unique_ptr<Expression> &&argument);
  ApplicationExpression();
  void accept(ExpressionVisitor &visitor) const override;
  void accept(MutatingExpressionVisitor &visitor) override;
  constexpr std::unique_ptr<Expression> &function() { return children[0]; }
  constexpr std::unique_ptr<Expression> &argument() { return children[1]; }

 private:
  static std::shared_ptr<spdlog::logger> get_logger();
};

} // namespace script3025

#endif
