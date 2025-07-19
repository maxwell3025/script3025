#include "expression/subtypes/equality_expression.hpp"

namespace script3025 {

EqualityExpression::EqualityExpression(
    std::unique_ptr<Expression> &&lhs,
    std::unique_ptr<Expression> &&rhs)
    : Expression{std::move(lhs), std::move(rhs)} {}

EqualityExpression::EqualityExpression() {}

void EqualityExpression::accept(ExpressionVisitor &visitor) const {
    visitor.visit_equality(*this);
}

void EqualityExpression::accept(MutatingExpressionVisitor &visitor) {
    visitor.visit_equality(*this);
}

std::shared_ptr<spdlog::logger> EqualityExpression::get_logger() {
  static std::shared_ptr<spdlog::logger> logger =
      ([&] () -> std::shared_ptr<spdlog::logger> {
        logger = spdlog::stderr_color_mt("script3025::EqualityExpression", spdlog::color_mode::always);
        logger-> set_level(spdlog::level::warn);
        logger-> set_pattern("%^[%l] [tid=%t] [%T.%F] [%s:%#] %v%$");
        return logger;
      })();
  return logger;
}

}
