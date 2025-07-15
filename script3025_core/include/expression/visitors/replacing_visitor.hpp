#ifndef SCRIPT3025_SCRIPT3025_CORE_REPLACING_VISITOR_HPP
#define SCRIPT3025_SCRIPT3025_CORE_REPLACING_VISITOR_HPP

#include "expression/expression_base.hpp"
#include "expression/expression_visitor.hpp"

namespace script3025 {

class ReplacingVisitor : public ExpressionVisitor {
 public:
  void visit_application(const ApplicationExpression &e) override;
  void visit_id(const IdExpression &e) override;
  void visit_lambda(const LambdaExpression &e) override;
  void visit_let(const LetExpression &e) override;
  void visit_pi(const PiExpression &e) override;

  Expression *target;
  Expression *replacement;
  std::unique_ptr<Expression> value;
  std::unordered_map<const Expression *, Expression *> pointer_map;
};

} // namespace script3025

#endif
