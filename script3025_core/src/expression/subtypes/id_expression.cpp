#include "expression/subtypes/id_expression.hpp"

#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <memory>
#include <string>
#include <utility>

#include "expression/expression.hpp"

namespace script3025 {

IdExpression::IdExpression(std::string id, ScopeExpression *source)
    : id(std::move(id)), source(source) {}

IdExpression::IdExpression() {}

std::shared_ptr<spdlog::logger> IdExpression::get_logger() {
  static std::shared_ptr<spdlog::logger> logger =
      ([&]() -> std::shared_ptr<spdlog::logger> {
        logger = spdlog::stderr_color_mt("script3025::IdExpression",
                                         spdlog::color_mode::always);
        logger->set_level(spdlog::level::warn);
        logger->set_pattern("%^[%l] [tid=%t] [%T.%F] [%s:%#] %v%$");
        return logger;
      })();
  return logger;
}

}  // namespace script3025
