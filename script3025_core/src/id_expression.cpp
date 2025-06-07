#include "expression.hpp"

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
      LOGGER3025_ERROR("Invalid source!");
    }
  }

  if (type) {
    return type.get();
  } else {
    LOGGER3025_ERROR("Could not find type for expression {}", to_string(*this));
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
}

} //namespace scritp3025
