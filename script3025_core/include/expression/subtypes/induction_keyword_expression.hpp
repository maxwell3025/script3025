#ifndef SCRIPT3025_SCRIPT3025_CORE_INDUCTION_KEYWORD_EXPRESSION_HPP
#define SCRIPT3025_SCRIPT3025_CORE_INDUCTION_KEYWORD_EXPRESSION_HPP

#include "expression/expression_base.hpp"
#include "expression/expression_visitor.hpp"

namespace script3025 {

class InductionKeywordExpression : public Expression {
 public:
  InductionKeywordExpression();
  void accept(ExpressionVisitor &visitor) const override;
  void accept(MutatingExpressionVisitor &visitor) override;
  virtual std::vector<Expression *> get_children() const override;

 private:
  static std::shared_ptr<spdlog::logger> get_logger();
};

} // namespace script3025

#endif
