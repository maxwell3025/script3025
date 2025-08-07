#include "expression/subtypes/nat_literal_expression.hpp"

#include <gmpxx.h>
#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <memory>

namespace script3025 {

NatLiteralExpression::NatLiteralExpression(mpz_class value) : value_(value) {}

NatLiteralExpression::NatLiteralExpression() : value_(0) {}

std::shared_ptr<spdlog::logger> NatLiteralExpression::get_logger() {
  static std::shared_ptr<spdlog::logger> logger =
      ([&]() -> std::shared_ptr<spdlog::logger> {
        logger = spdlog::stderr_color_mt("script3025::NatLiteralExpression",
                                         spdlog::color_mode::always);
        logger->set_level(spdlog::level::warn);
        logger->set_pattern("%^[%l] [tid=%t] [%T.%F] [%s:%#] %v%$");
        return logger;
      })();
  return logger;
}

}  // namespace script3025
