#include "expression/subtypes/succ_keyword_expression.hpp"

namespace script3025 {

std::shared_ptr<spdlog::logger> SuccKeywordExpression::get_logger() {
  static std::shared_ptr<spdlog::logger> logger =
      ([&] () -> std::shared_ptr<spdlog::logger> {
        logger = spdlog::stderr_color_mt("script3025::SuccKeywordExpression", spdlog::color_mode::always);
        logger-> set_level(spdlog::level::warn);
        logger-> set_pattern("%^[%l] [tid=%t] [%T.%F] [%s:%#] %v%$");
        return logger;
      })();
  return logger;
}

void SuccKeywordExpression::accept(ExpressionVisitor &visitor) const {
    visitor.visit_succ_keyword(*this);
}

void SuccKeywordExpression::accept(MutatingExpressionVisitor &visitor) {
    visitor.visit_succ_keyword(*this);
}

std::vector<Expression *> SuccKeywordExpression::get_children() const {
  return {};
}

}
