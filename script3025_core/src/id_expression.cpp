#include "id_expression.hpp"
#include "pi_expression.hpp"
#include "lambda_expression.hpp"

namespace script3025 {

IdExpression::IdExpression(std::string &&id, Expression *source)
                           : id(std::move(id)), source(source) {}

IdExpression::IdExpression(const IdExpression &other)
    : id(other.id), source(other.source), Expression(other) {}

IdExpression::IdExpression() {}

bool IdExpression::is_normal() const {
  // Currently this is always true, although this needs to be changed once delta
  // reductions and let expressions are implemented.
  return true;
}

std::ostream &IdExpression::print(std::ostream &os) const {
  return os << id;
}

std::shared_ptr<spdlog::logger> IdExpression::get_logger() {
  static std::shared_ptr<spdlog::logger> logger =
      ([&] () -> std::shared_ptr<spdlog::logger> {
        logger = spdlog::stderr_color_mt("script3025::IdExpression", spdlog::color_mode::always);
        logger-> set_level(spdlog::level::warn);
        logger-> set_pattern("%^[%l] [tid=%t] [%T.%F] [%s:%#] %v%$");
        return logger;
      })();
  return logger;
}

void IdExpression::accept(ExpressionVisitor &visitor) const {
  visitor.visit_id(*this);
}

void IdExpression::accept(MutatingExpressionVisitor &visitor) {
  visitor.visit_id(*this);
}

} //namespace scritp3025
