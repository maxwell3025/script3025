#ifndef SCRIPT3025_CORE_INCLUDE_EXPRESSION_VISITORS_SCOPE_HYGIENE_VISITOR_HPP
#define SCRIPT3025_CORE_INCLUDE_EXPRESSION_VISITORS_SCOPE_HYGIENE_VISITOR_HPP

#include "expression/expression.hpp"
#include "expression/expression_visitor.hpp"
#include "expression/visitors/scope_walking_visitor.hpp"

namespace script3025 {

class ScopeHygieneVisitor : public ScopeWalkingVisitor {
 public:
  void visit_id(IdExpression &e) override {
    if (!e.source) return;
    for (size_t i = lexical_scope_[e.id].size(); i-- != 0;) {
      if (lexical_scope_[e.id][i] == e.source) return;
    }
    is_sanitary = false;
  }

  bool get() { return is_sanitary; }

 protected:
  bool is_sanitary = true;
};

bool is_hygenic(const Expression &e) {
  ScopeHygieneVisitor visitor;
  // TODO: this is rather ugly and unprincipled.
  // This should be solved with a deep refactoring where the base visitor
  // classes and ScopeWalkingVisitor are made into templates over constness.
  // use std::add_const and std::conditional and use a bool to switch.
  visitor.visit(const_cast<Expression &>(e));
  return visitor.get();
}

}  // namespace script3025

#endif
