#include "expression/subtypes/application_expression.hpp"

#include "expression/subtypes/id_expression.hpp"

namespace script3025 {

ApplicationExpression::ApplicationExpression(
    std::unique_ptr<Expression> &&function,
    std::unique_ptr<Expression> &&argument) {
  children.emplace_back(std::move(function));
  children.emplace_back(std::move(argument));
}

ApplicationExpression::ApplicationExpression() {}

std::shared_ptr<spdlog::logger> ApplicationExpression::get_logger() {
  static std::shared_ptr<spdlog::logger> logger =
      ([&]() -> std::shared_ptr<spdlog::logger> {
        logger = spdlog::stderr_color_mt("script3025::ApplicationExpression",
                                         spdlog::color_mode::always);
        logger->set_level(spdlog::level::warn);
        logger->set_pattern("%^[%l] [tid=%t] [%T.%F] [%s:%#] %v%$");
        return logger;
      })();
  return logger;
}

}  // namespace script3025
