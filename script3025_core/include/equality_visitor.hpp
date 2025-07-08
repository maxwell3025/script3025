#ifndef SCRIPT3025_SCRIPT3025_CORE_EQUALITY_VISITOR_HPP
#define SCRIPT3025_SCRIPT3025_CORE_EQUALITY_VISITOR_HPP

#include "expression.hpp"
#include "expression_visitor.hpp"

namespace script3025 {

class EqualityVisitor : public ExpressionVisitor {
 public:
  EqualityVisitor();
  void visit_application(const ApplicationExpression &rhs) override;
  void visit_id(const IdExpression &rhs) override;
  void visit_lambda(const LambdaExpression &rhs) override;
  void visit_let(const LetExpression &rhs) override;
  void visit_pi(const PiExpression &rhs) override;

  bool unequal;
  const Expression *lhs;
  std::unordered_map<const Expression *, const Expression *> pointer_map;
};

} // namespace script3025

#endif
