#include "expression/subtypes/application_expression.hpp"

#include "expression/subtypes/id_expression.hpp"

namespace script3025 {

ApplicationExpression::ApplicationExpression(
    std::unique_ptr<Expression> &&function,
    std::unique_ptr<Expression> &&argument)
    : function(std::move(function)),
    argument(std::move(argument)) {}

ApplicationExpression::ApplicationExpression(const ApplicationExpression &other)
    : function(other.function -> clone()),
    argument(other.argument -> clone())
    {}

ApplicationExpression::ApplicationExpression() {}

std::shared_ptr<spdlog::logger> ApplicationExpression::get_logger() {
  static std::shared_ptr<spdlog::logger> logger =
      ([&] () -> std::shared_ptr<spdlog::logger> {
        logger = spdlog::stderr_color_mt("script3025::ApplicationExpression", spdlog::color_mode::always);
        logger-> set_level(spdlog::level::warn);
        logger-> set_pattern("%^[%l] [tid=%t] [%T.%F] [%s:%#] %v%$");
        return logger;
      })();
  return logger;
}

void ApplicationExpression::accept(ExpressionVisitor &visitor) const {
  visitor.visit_application(*this);
}

void ApplicationExpression::accept(MutatingExpressionVisitor &visitor) {
  visitor.visit_application(*this);
}

std::vector<Expression *> ApplicationExpression::get_children() const {
  return {function.get(), argument.get()};
}

}
