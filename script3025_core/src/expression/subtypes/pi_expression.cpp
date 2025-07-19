#include "expression/subtypes/pi_expression.hpp"

namespace script3025 {

PiExpression::PiExpression(std::string argument_id,
                           std::unique_ptr<Expression> &&argument_type,
                           std::unique_ptr<Expression> &&definition)
                           : argument_id(argument_id),
                           Expression{std::move(argument_type),
                                      std::move(definition)} {}

PiExpression::PiExpression() {}

void PiExpression::accept(ExpressionVisitor &visitor) const {
  visitor.visit_pi(*this);
}

void PiExpression::accept(MutatingExpressionVisitor &visitor) {
  visitor.visit_pi(*this);
}

std::shared_ptr<spdlog::logger> PiExpression::get_logger() {
  static std::shared_ptr<spdlog::logger> logger =
      ([&] () -> std::shared_ptr<spdlog::logger> {
        logger = spdlog::stderr_color_mt("script3025::PiExpression", spdlog::color_mode::always);
        logger-> set_level(spdlog::level::warn);
        logger-> set_pattern("%^[%l] [tid=%t] [%T.%F] [%s:%#] %v%$");
        return logger;
      })();
  return logger;
}

}
