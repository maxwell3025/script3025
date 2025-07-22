#include "expression/subtypes/reflexive_keyword_expression.hpp"

namespace script3025 {

void ReflexiveKeywordExpression::accept(ExpressionVisitor &visitor) const {
  visitor.visit_reflexive_keyword(*this);
}

void ReflexiveKeywordExpression::accept(MutatingExpressionVisitor &visitor) {
  visitor.visit_reflexive_keyword(*this);
}

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
