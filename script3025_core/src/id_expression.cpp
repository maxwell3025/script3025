#include "id_expression.hpp"
#include "pi_expression.hpp"
#include "lambda_expression.hpp"

namespace script3025 {

IdExpression::IdExpression(std::string &&id, Expression *source)
                           : id(std::move(id)), source(source),
                           Expression(source, std::unique_ptr<Expression>(),
                           true) {}

IdExpression::IdExpression(const IdExpression &other)
    : id(other.id), source(other.source), Expression(other) {}

std::unique_ptr<Expression> IdExpression::clone() const {
  return std::make_unique<IdExpression>(*this);
}

bool IdExpression::is_normal() const {
  // Currently this is always true, although this needs to be changed once delta
  // reductions and let expressions are implemented.
  return true;
}

Expression* IdExpression::get_type() {
  if (possibly_well_typed && !type){
    if (!source) {
      // TODO: if our id is floating, then in theory it should still be typeable
      // if it is a hard-coded constant.
      possibly_well_typed = false;
    } else if (typeid(*source) == typeid(LambdaExpression)) {
      type = static_cast<LambdaExpression*>(source) -> argument_type -> reduce();
    } else if (typeid(*source) == typeid(PiExpression)) {
      type = static_cast<PiExpression*>(source) -> argument_type -> reduce();
    } else {
      SPDLOG_LOGGER_ERROR(get_logger(), "Invalid source!");
    }
  }

  if (type) {
    return type.get();
  } else {
    SPDLOG_LOGGER_ERROR(get_logger(), "Could not find type for expression {}", to_string());
    return nullptr;
  }
}

std::unique_ptr<Expression> IdExpression::replace(
    const std::string &id,
    const Expression *source,
    const Expression &expression) {
  if (this -> id == id && this -> source == source) {
    return expression.clone();
  }
  return clone();
}

std::unique_ptr<Expression> IdExpression::reduce() {
  // TODO implement delta reductions
  return clone();
}

bool IdExpression::operator==(const Expression &other) const {
  if(typeid(other) != typeid(IdExpression)) return false;
  const IdExpression &other_id =
      static_cast<const IdExpression &>(other);
  return id == other_id.id &&
         source == other_id.source;
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
