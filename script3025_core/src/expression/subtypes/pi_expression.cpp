#include "expression/subtypes/pi_expression.hpp"

namespace script3025 {

PiExpression::PiExpression(std::string &&argument_id,
                           std::unique_ptr<Expression> &&argument_type,
                           std::unique_ptr<Expression> &&definition)
                           : argument_id(argument_id),
                           argument_type(std::move(argument_type)),
                           definition(std::move(definition)) {}

PiExpression::PiExpression(const PiExpression &other)
    : argument_id(other.argument_id),
    argument_type(other.argument_type -> clone()),
    definition(other.definition -> clone()) {}

PiExpression::PiExpression() {}

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

void PiExpression::accept(ExpressionVisitor &visitor) const {
  visitor.visit_pi(*this);
}

void PiExpression::accept(MutatingExpressionVisitor &visitor) {
  visitor.visit_pi(*this);
}

}
