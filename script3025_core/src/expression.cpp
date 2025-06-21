#include "expression.hpp"

namespace script3025 {

Expression::Expression(Expression *parent_abstraction,
                       std::unique_ptr<Expression> &&type,
                       bool possibly_well_typed)
                       : parent_abstraction(parent_abstraction),
                       type(std::move(type)),
                       possibly_well_typed(possibly_well_typed) {}

Expression::Expression(const Expression& other)
                       : parent_abstraction(other.parent_abstraction),
                       type(other.type -> clone()),
                       possibly_well_typed(other.possibly_well_typed) {}

Expression::Expression() : parent_abstraction(nullptr),
                         type(),
                         possibly_well_typed(true) {}

bool Expression::operator!=(const Expression& rhs) const {
  return !(*this == rhs);
}

std::shared_ptr<spdlog::logger> Expression::get_logger() {
  static std::shared_ptr<spdlog::logger> logger =
      ([&] () -> std::shared_ptr<spdlog::logger> {
        logger = spdlog::stderr_color_mt("script3025::Expression", spdlog::color_mode::always);
        logger-> set_level(spdlog::level::warn);
        logger-> set_pattern("%^[%l] [tid=%t] [%T.%F] [%s:%#] %v%$");
        return logger;
      })();
  return logger;
}

std::string Expression::to_string() {
  std::stringstream output;
  output << *this;
  return output.str();
}

} //namespace script3025