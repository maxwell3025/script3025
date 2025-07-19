#include "expression/subtypes/nat_keyword_expression.hpp"

namespace script3025 {

void NatKeywordExpression::accept(ExpressionVisitor &visitor) const {
    visitor.visit_nat_keyword(*this);
}

void NatKeywordExpression::accept(MutatingExpressionVisitor &visitor) {
    visitor.visit_nat_keyword(*this);
}

std::shared_ptr<spdlog::logger> NatKeywordExpression::get_logger() {
  static std::shared_ptr<spdlog::logger> logger =
      ([&] () -> std::shared_ptr<spdlog::logger> {
        logger = spdlog::stderr_color_mt("script3025::TypeLiteralExpression", spdlog::color_mode::always);
        logger-> set_level(spdlog::level::warn);
        logger-> set_pattern("%^[%l] [tid=%t] [%T.%F] [%s:%#] %v%$");
        return logger;
      })();
  return logger;
}

}
