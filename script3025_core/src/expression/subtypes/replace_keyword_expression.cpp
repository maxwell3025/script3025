#include "expression/subtypes/replace_keyword_expression.hpp"

namespace script3025 {

std::shared_ptr<spdlog::logger> ReplaceKeywordExpression::get_logger() {
  static std::shared_ptr<spdlog::logger> logger =
      ([&] () -> std::shared_ptr<spdlog::logger> {
        logger = spdlog::stderr_color_mt("script3025::ReplaceKeywordExpression", spdlog::color_mode::always);
        logger-> set_level(spdlog::level::warn);
        logger-> set_pattern("%^[%l] [tid=%t] [%T.%F] [%s:%#] %v%$");
        return logger;
      })();
  return logger;
}

void ReplaceKeywordExpression::accept(ExpressionVisitor &visitor) const {
    visitor.visit_replace_keyword(*this);
}

void ReplaceKeywordExpression::accept(MutatingExpressionVisitor &visitor) {
    visitor.visit_replace_keyword(*this);
}

std::vector<Expression *> ReplaceKeywordExpression::get_children() const {
  return {};
}

}
