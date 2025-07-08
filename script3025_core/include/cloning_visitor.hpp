#ifndef SCRIPT3025_SCRIPT3025_CORE_CLONING_VISITOR_HPP
#define SCRIPT3025_SCRIPT3025_CORE_CLONING_VISITOR_HPP

#include "expression.hpp"
#include "expression_visitor.hpp"

namespace script3025 {

class CloningVisitor : public ExpressionVisitor {
 public:
  void visit_application(const ApplicationExpression &e) override;
  void visit_id(const IdExpression &e) override;
  void visit_lambda(const LambdaExpression &e) override;
  void visit_let(const LetExpression &e) override;
  void visit_pi(const PiExpression &e) override;

  std::unique_ptr<Expression> value;
  std::unordered_map<const Expression *, Expression *> pointer_map;
};

} // namespace script3025

#endif
