#ifndef SCRIPT3025_CORE_EXPRESSION_SUBTYPES_REFLEXIVE_KEYWORD_EXPRESSION_HPP
#define SCRIPT3025_CORE_EXPRESSION_SUBTYPES_REFLEXIVE_KEYWORD_EXPRESSION_HPP

#include <memory>

#include "expression/expression_visitor.hpp"
#include "expression/subtypes/keyword_expression.hpp"
#include "spdlog/logger.h"

namespace script3025 {

class ReflexiveKeywordExpression : public KeywordExpression {
 public:
  void accept(ExpressionVisitor<true> &visitor) const override {
    visitor.visit_reflexive_keyword(*this);
  }
  void accept(ExpressionVisitor<false> &visitor) override {
    visitor.visit_reflexive_keyword(*this);
  }

 private:
  [[nodiscard]] static std::shared_ptr<spdlog::logger> get_logger();
};

}  // namespace script3025

#endif
