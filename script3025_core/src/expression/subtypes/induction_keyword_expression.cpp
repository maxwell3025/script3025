#include "expression/subtypes/induction_keyword_expression.hpp"

namespace script3025 {

std::shared_ptr<spdlog::logger> InductionKeywordExpression::get_logger() {
  static std::shared_ptr<spdlog::logger> logger =
      ([&] () -> std::shared_ptr<spdlog::logger> {
        logger = spdlog::stderr_color_mt("script3025::InductionKeywordExpression", spdlog::color_mode::always);
        logger-> set_level(spdlog::level::warn);
        logger-> set_pattern("%^[%l] [tid=%t] [%T.%F] [%s:%#] %v%$");
        return logger;
      })();
  return logger;
}

void InductionKeywordExpression::accept(ExpressionVisitor &visitor) const {
    visitor.visit_induction_keyword(*this);
}

void InductionKeywordExpression::accept(MutatingExpressionVisitor &visitor) {
    visitor.visit_induction_keyword(*this);
}

std::vector<Expression *> InductionKeywordExpression::get_children() const {
  return {};
}

}
