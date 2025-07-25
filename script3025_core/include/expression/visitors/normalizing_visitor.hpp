#ifndef SCRIPT3025_CORE_EXPRESSION_VISITORS_NORMALIZING_VISITOR_HPP
#define SCRIPT3025_CORE_EXPRESSION_VISITORS_NORMALIZING_VISITOR_HPP

#include "expression/expression.hpp"
#include "expression/expression_visitor.hpp"
#include "expression/visitors/scope_walking_visitor.hpp"

namespace script3025 {

class NormalizingVisitor : public ScopeWalkingVisitor {
 public:
  void visit_id(IdExpression &e) override { e.source = get_source(e.id); }
};

}  // namespace script3025

#endif
