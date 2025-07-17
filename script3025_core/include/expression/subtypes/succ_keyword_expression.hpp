#ifndef SCRIPT3025_SCRIPT3025_CORE_SUCC_KEYWORD_EXPRESSION_HPP
#define SCRIPT3025_SCRIPT3025_CORE_SUCC_KEYWORD_EXPRESSION_HPP

#include "expression/expression_base.hpp"
#include "expression/expression_visitor.hpp"

namespace script3025 {

class SuccKeywordExpression : public Expression {
 public:
  SuccKeywordExpression();
  void accept(ExpressionVisitor &visitor) const override;
  void accept(MutatingExpressionVisitor &visitor) override;

 private:
  static std::shared_ptr<spdlog::logger> get_logger();
};

} // namespace script3025

#endif
