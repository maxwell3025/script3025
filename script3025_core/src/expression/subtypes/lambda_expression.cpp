#include "expression/subtypes/lambda_expression.hpp"

namespace script3025 {

LambdaExpression::LambdaExpression(std::string argument_id,
                                   std::unique_ptr<Expression> &&argument_type,
                                   std::unique_ptr<Expression> &&definition)
                                   : argument_id(std::move(argument_id)),
                                   Expression{std::move(argument_type),
                                              std::move(definition)} {}

LambdaExpression::LambdaExpression() {}

void LambdaExpression::accept(ExpressionVisitor &visitor) const {
  visitor.visit_lambda(*this);
}

void LambdaExpression::accept(MutatingExpressionVisitor &visitor) {
  visitor.visit_lambda(*this);
}

std::shared_ptr<spdlog::logger> LambdaExpression::get_logger() {
  static std::shared_ptr<spdlog::logger> logger =
      ([&] () -> std::shared_ptr<spdlog::logger> {
        logger = spdlog::stderr_color_mt("script3025::LambdaExpression", spdlog::color_mode::always);
        logger-> set_level(spdlog::level::warn);
        logger-> set_pattern("%^[%l] [tid=%t] [%T.%F] [%s:%#] %v%$");
        return logger;
      })();
  return logger;
}

}
