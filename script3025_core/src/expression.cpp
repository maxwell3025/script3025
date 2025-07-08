#include "expression.hpp"
#include "equality_visitor.hpp"
#include "cloning_visitor.hpp"

namespace script3025 {

std::unique_ptr<Expression> Expression::clone(
    std::unordered_map<const Expression *, Expression *> initial_map) {
  CloningVisitor visitor;
  visitor.pointer_map = initial_map;
  accept(visitor);
  return std::move(visitor.value);
}

bool Expression::operator==(const Expression& rhs) const {
  return !(*this != rhs);
}

bool Expression::operator!=(const Expression& rhs) const {
  EqualityVisitor visitor;
  visitor.lhs = this;
  visitor.unequal = false;
  rhs.accept(visitor);
  return visitor.unequal;
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