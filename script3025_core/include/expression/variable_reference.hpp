#ifndef SCRIPT3025_SCRIPT3025_CORE_VARIABLE_REFERENCE
#define SCRIPT3025_SCRIPT3025_CORE_VARIABLE_REFERENCE

#include <functional>

#include "expression/subtypes/scope_expression.hpp"

namespace script3025 {

// @brief Syntactic reference to a variable.
// @example Global/unbound variables have `nullptr` as their source.
struct VariableReference {
  // The name of the identifier.
  std::string id;
  // The expression that introduces the identifier.
  ScopeExpression *source;

  bool operator==(const VariableReference &rhs) const {
    return id == rhs.id && source == rhs.source;
  }
};

}  // namespace script3025

template <>
struct std::hash<script3025::VariableReference> {
  size_t operator()(const script3025::VariableReference &ref) const {
    return ((std::hash<std::string>{}(ref.id)) << 1) ^
           ((std::hash<script3025::ScopeExpression *>{}(ref.source)));
  }
};

#endif