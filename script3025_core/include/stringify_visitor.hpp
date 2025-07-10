#ifndef SCRIPT3025_SCRIPT3025_CORE_STRINGIFY_VISITOR_HPP
#define SCRIPT3025_SCRIPT3025_CORE_STRINGIFY_VISITOR_HPP

#include "expression.hpp"
#include "expression_visitor.hpp"

namespace script3025 {

class StringifyVisitor : public ExpressionVisitor {
 public:
  void visit_application(const ApplicationExpression &e) override;
  void visit_id(const IdExpression &e) override;
  void visit_lambda(const LambdaExpression &e) override;
  void visit_let(const LetExpression &e) override;
  void visit_pi(const PiExpression &e) override;
};

} // namespace script3025

#endif
