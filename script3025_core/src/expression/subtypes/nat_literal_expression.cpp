#include "expression/subtypes/nat_literal_expression.hpp"

namespace script3025 {

std::shared_ptr<spdlog::logger> NatLiteralExpression::get_logger() {
  static std::shared_ptr<spdlog::logger> logger =
      ([&] () -> std::shared_ptr<spdlog::logger> {
        logger = spdlog::stderr_color_mt("script3025::TypeLiteralExpression", spdlog::color_mode::always);
        logger-> set_level(spdlog::level::warn);
        logger-> set_pattern("%^[%l] [tid=%t] [%T.%F] [%s:%#] %v%$");
        return logger;
      })();
  return logger;
}

void NatLiteralExpression::accept(ExpressionVisitor &visitor) const {
    visitor.visit_nat_literal(*this);
}

void NatLiteralExpression::accept(MutatingExpressionVisitor &visitor) {
    visitor.visit_nat_literal(*this);
}

}
