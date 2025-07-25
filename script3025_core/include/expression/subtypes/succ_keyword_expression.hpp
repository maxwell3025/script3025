#ifndef SCRIPT3025_SCRIPT3025_CORE_SUCC_KEYWORD_EXPRESSION_HPP
#define SCRIPT3025_SCRIPT3025_CORE_SUCC_KEYWORD_EXPRESSION_HPP

#include "expression/expression_visitor.hpp"
#include "expression/subtypes/keyword_expression.hpp"

namespace script3025 {

class SuccKeywordExpression : public KeywordExpression {
 public:
  void accept(ExpressionVisitor<true> &visitor) const override { visitor.visit_succ_keyword(*this); }
  void accept(ExpressionVisitor<false> &visitor) override { visitor.visit_succ_keyword(*this); }

 private:
  static std::shared_ptr<spdlog::logger> get_logger();
};

}  // namespace script3025

#endif
