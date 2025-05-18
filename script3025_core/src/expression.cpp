#include "expression.hpp"

namespace script3025 {

bool Expression::operator!=(const Expression& rhs) const {
  return !(*this == rhs);
}

}