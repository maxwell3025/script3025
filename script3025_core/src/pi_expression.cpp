#include "expression.hpp"

namespace script3025 {

PiExpression::PiExpression(std::string &&argument_id,
                           std::unique_ptr<Expression> &&argument_type,
                           std::unique_ptr<Expression> &&definition)
                           : argument_id(argument_id),
                           argument_type(std::move(argument_type)),
                           definition(std::move(definition)),
                           Expression(argument_type -> parent_abstraction,
                                      std::unique_ptr<Expression>(), true) {
  definition -> parent_abstraction = this;
}

PiExpression::PiExpression(const PiExpression &other)
    : argument_id(other.argument_id),
    argument_type(other.argument_type -> clone()),
    definition(other.definition -> clone()), Expression(other) {}

std::unique_ptr<Expression> PiExpression::clone() const {
  return std::make_unique<PiExpression>(*this);
}

bool PiExpression::is_normal() const {
  return argument_type -> is_normal() && definition -> is_normal();
}

Expression *PiExpression::get_type() {
  // TODO figure out how to handle type levels

  if (type) {
    return type.get();
  } else {
    LOGGER3025_ERROR("Could not find type for expression {}", to_string(*this));
    return nullptr;
  }
}

std::unique_ptr<Expression> PiExpression::replace(
    const std::string &id, const Expression *source,
    const Expression &expression) {
  std::unique_ptr<PiExpression> output =
      std::make_unique<PiExpression>(*this);

  output -> argument_type =
      output -> argument_type -> replace(id, source, expression);
  output -> definition =
      output -> definition -> replace(id, source, expression);

  return output;
}

std::unique_ptr<Expression> PiExpression::reduce() {
  std::unique_ptr<PiExpression> output =
      std::make_unique<PiExpression>(*this);
  output -> definition = output -> definition -> reduce();
  output -> argument_type = output -> argument_type -> reduce();
  return output;
};

bool PiExpression::operator==(const Expression &other) const {
  if(typeid(other) != typeid(PiExpression)) return false;
  const PiExpression &other_lambda =
      static_cast<const PiExpression &>(other);
  return argument_id == other_lambda.argument_id &&
         *argument_type == *other_lambda.argument_type &&
         *definition == *other_lambda.definition;
}

std::ostream &PiExpression::print(std::ostream &os) const {
  return os << "Π (" << argument_id << " : " << *argument_type << "). " <<
        *definition;
}

}
