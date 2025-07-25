#include "expression/subtypes/equality_expression.hpp"

namespace script3025 {

EqualityExpression::EqualityExpression(std::unique_ptr<Expression> &&lhs,
                                       std::unique_ptr<Expression> &&rhs) {
  children.emplace_back(std::move(lhs));
  children.emplace_back(std::move(rhs));
}

EqualityExpression::EqualityExpression() {}

std::shared_ptr<spdlog::logger> EqualityExpression::get_logger() {
  static std::shared_ptr<spdlog::logger> logger =
      ([&]() -> std::shared_ptr<spdlog::logger> {
        logger = spdlog::stderr_color_mt("script3025::EqualityExpression",
                                         spdlog::color_mode::always);
        logger->set_level(spdlog::level::warn);
        logger->set_pattern("%^[%l] [tid=%t] [%T.%F] [%s:%#] %v%$");
        return logger;
      })();
  return logger;
}

}  // namespace script3025
