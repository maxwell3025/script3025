#include "expression/subtypes/application_expression.hpp"

#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <memory>
#include <utility>

#include "expression/expression.hpp"

namespace script3025 {

ApplicationExpression::ApplicationExpression(
    std::unique_ptr<Expression> &&function,
    std::unique_ptr<Expression> &&argument) {
  children.emplace_back(std::move(function));
  children.emplace_back(std::move(argument));
}

ApplicationExpression::ApplicationExpression() {
  children.emplace_back(nullptr);
  children.emplace_back(nullptr);
}

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
