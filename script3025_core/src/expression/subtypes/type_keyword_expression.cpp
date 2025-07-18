#include "expression/subtypes/type_keyword_expression.hpp"

namespace script3025 {

std::shared_ptr<spdlog::logger> TypeKeywordExpression::get_logger() {
  static std::shared_ptr<spdlog::logger> logger =
      ([&] () -> std::shared_ptr<spdlog::logger> {
        logger = spdlog::stderr_color_mt("script3025::TypeKeywordExpression", spdlog::color_mode::always);
        logger-> set_level(spdlog::level::warn);
        logger-> set_pattern("%^[%l] [tid=%t] [%T.%F] [%s:%#] %v%$");
        return logger;
      })();
  return logger;
}

void TypeKeywordExpression::accept(ExpressionVisitor &visitor) const {
    visitor.visit_type_keyword(*this);
}

void TypeKeywordExpression::accept(MutatingExpressionVisitor &visitor) {
    visitor.visit_type_keyword(*this);
}

std::vector<Expression *> TypeKeywordExpression::get_children() const {
  return {};
}

}
