#ifndef SCRIPT3025_SCRIPT3025_CORE_APPLICATION_EXPRESSION_HPP
#define SCRIPT3025_SCRIPT3025_CORE_APPLICATION_EXPRESSION_HPP

#include "expression.hpp"
#include "expression_visitor.hpp"

namespace script3025 {

class ApplicationExpression : public Expression {
 public:
  ApplicationExpression(std::unique_ptr<Expression> &&function,
                        std::unique_ptr<Expression> &&argument);
  ApplicationExpression(const ApplicationExpression &other);
  ApplicationExpression();
  bool is_normal() const override;
  std::ostream &print(std::ostream &os) const override;
  void accept(ExpressionVisitor &visitor) const override;
  void accept(MutatingExpressionVisitor &visitor) override;

  std::unique_ptr<Expression> function;
  std::unique_ptr<Expression> argument;
 private:
  /**
   * @brief converts an application expression into weak-head normal form by
   * recursively applying.
   * 
   * For example, if you have the expression F a b c d, this will first
   * apply a, then b, then c, then d.
   * 
   * this uses lazy evaluation, since a b c d are not reduced to normal form
   * first.
  */
  std::unique_ptr<Expression> get_whnf();

  static std::shared_ptr<spdlog::logger> get_logger();
};

} // namespace script3025

#endif
