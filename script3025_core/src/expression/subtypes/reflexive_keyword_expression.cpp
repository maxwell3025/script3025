#include "expression/subtypes/reflexive_keyword_expression.hpp"

#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <memory>

namespace script3025 {

std::shared_ptr<spdlog::logger> ReflexiveKeywordExpression::get_logger() {
  static std::shared_ptr<spdlog::logger> logger =
      ([&]() -> std::shared_ptr<spdlog::logger> {
        logger =
            spdlog::stderr_color_mt("script3025::ReflexiveKeywordExpression",
                                    spdlog::color_mode::always);
        logger->set_level(spdlog::level::warn);
        logger->set_pattern("%^[%l] [tid=%t] [%T.%F] [%s:%#] %v%$");
        return logger;
      })();
  return logger;
}

}  // namespace script3025
