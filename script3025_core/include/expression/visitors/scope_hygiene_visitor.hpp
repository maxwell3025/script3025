#ifndef SCRIPT3025_CORE_INCLUDE_EXPRESSION_VISITORS_SCOPE_HYGIENE_VISITOR_HPP
#define SCRIPT3025_CORE_INCLUDE_EXPRESSION_VISITORS_SCOPE_HYGIENE_VISITOR_HPP

#include "expression/expression.hpp"
#include "expression/expression_visitor.hpp"
#include "expression/visitors/scope_walking_visitor.hpp"

namespace script3025 {

class ScopeHygieneVisitor : public ScopeWalkingVisitor<true> {
 public:
  void visit_id(const IdExpression &e) override {
    if (!e.source) return;
    for (size_t i = lexical_scope_[e.id].size(); i-- != 0;) {
      if (lexical_scope_[e.id][i] == e.source) return;
    }
    is_sanitary = false;
  }

  [[nodiscard]] bool get() const { return is_sanitary; }

 protected:
  bool is_sanitary = true;
};

[[nodiscard]] inline bool is_hygenic(const Expression &e) {
  ScopeHygieneVisitor visitor;
  e.accept(visitor);
  return visitor.get();
}

}  // namespace script3025

#endif
