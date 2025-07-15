#ifndef SCRIPT3025_SCRIPT3025_CORE_LAZY_REDUCTION_VISITOR
#define SCRIPT3025_SCRIPT3025_CORE_LAZY_REDUCTION_VISITOR

#include "expression/expression_base.hpp"
#include "expression/expression_visitor.hpp"

namespace script3025 {

class LazyReductionVisitor : public MutatingExpressionVisitor {
 public:
  void visit_application(ApplicationExpression &rhs) override;
  void visit_id(IdExpression &rhs) override;
  void visit_lambda(LambdaExpression &rhs) override;
  void visit_let(LetExpression &rhs) override;
  void visit_pi(PiExpression &rhs) override;
  
  std::unique_ptr<Expression> reduced_expression;
};

} // namespace script3025

#endif
