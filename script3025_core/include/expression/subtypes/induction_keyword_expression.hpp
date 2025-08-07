#ifndef SCRIPT3025_SCRIPT3025_CORE_INDUCTION_KEYWORD_EXPRESSION_HPP
#define SCRIPT3025_SCRIPT3025_CORE_INDUCTION_KEYWORD_EXPRESSION_HPP

#include <memory>

#include "expression/expression_visitor.hpp"
#include "expression/subtypes/keyword_expression.hpp"
#include "spdlog/logger.h"

namespace script3025 {

class InductionKeywordExpression : public KeywordExpression {
 public:
  void accept(ExpressionVisitor<true> &visitor) const override {
    visitor.visit_induction_keyword(*this);
  }
  void accept(ExpressionVisitor<false> &visitor) override {
    visitor.visit_induction_keyword(*this);
  }

 private:
  [[nodiscard]] static std::shared_ptr<spdlog::logger> get_logger();
};

}  // namespace script3025

#endif
