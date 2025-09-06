#include "expression/subtypes/lambda_expression.hpp"

#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <memory>
#include <string>
#include <utility>

#include "expression/expression.hpp"
#include "expression/subtypes/scope_expression.hpp"

namespace script3025 {

LambdaExpression::LambdaExpression(std::string argument_id,
                                   std::unique_ptr<Expression> &&argument_type,
                                   std::unique_ptr<Expression> &&definition)
    : ScopeExpression(std::move(argument_id)) {
  children.emplace_back(std::move(argument_type));
  children.emplace_back(std::move(definition));
}

LambdaExpression::LambdaExpression() {}

std::shared_ptr<spdlog::logger> LambdaExpression::get_logger() {
  static std::shared_ptr<spdlog::logger> logger =
      ([&]() -> std::shared_ptr<spdlog::logger> {
        logger = spdlog::stderr_color_mt("script3025::LambdaExpression",
                                         spdlog::color_mode::always);
        logger->set_level(spdlog::level::warn);
        logger->set_pattern("%^[%l] [tid=%t] [%T.%F] [%s:%#] %v%$");
        return logger;
      })();
  return logger;
}

}  // namespace script3025
