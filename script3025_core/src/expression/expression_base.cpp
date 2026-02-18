#include "expression/expression_base.hpp"

#include <spdlog/common.h>
#include <spdlog/fmt/bundled/base.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <memory>
#include <string>

#include "expression/expression.hpp"
#include "expression/expression_visitor.hpp"
#include "expression/visitors/cloning_visitor.hpp"
#include "expression/visitors/equality_visitor.hpp"
#include "expression/visitors/stringify_visitor.hpp"

namespace script3025 {

bool Expression::operator==(const Expression &rhs) const {
  EqualityVisitor visitor(this);
  rhs.accept(visitor);
  return visitor.get();
}

bool Expression::operator!=(const Expression &rhs) const {
  return !(*this == rhs);
}

[[nodiscard]] std::unique_ptr<Expression> Expression::clone(
    const std::unordered_map<const Expression *, Expression *> &initial_map)
    const {
  // TODO: Add the initial map to cloning visitor
  CloningVisitor visitor;
  accept(visitor);
  return visitor.get();
}

std::shared_ptr<spdlog::logger> Expression::get_logger() {
  static std::shared_ptr<spdlog::logger> logger =
      ([&]() -> std::shared_ptr<spdlog::logger> {
        logger = spdlog::stderr_color_mt("script3025::Expression",
                                         spdlog::color_mode::always);
        logger->set_level(spdlog::level::warn);
        logger->set_pattern("%^[%l] [tid=%t] [%T.%F] [%s:%#] %v%$");
        return logger;
      })();
  return logger;
}

std::string Expression::to_string() const {
  return stringify_expression(*this);
}

}  // namespace script3025

fmt::format_context::iterator fmt::formatter<script3025::Expression>::format(
    const script3025::Expression &expr, format_context &ctx) const {
  return fmt::format_to(ctx.out(), "{0}", expr.to_string());
}