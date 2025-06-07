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

std::ostream &IdExpression::print(std::ostream &os) const {
  return os << id;
}

} //namespace script3025