#ifndef SCRIPT3025_SCRIPT3025_CORE_EXPRESSION_VISITOR_HPP
#define SCRIPT3025_SCRIPT3025_CORE_EXPRESSION_VISITOR_HPP

namespace script3025 {

class ApplicationExpression;
class IdExpression;
class LambdaExpression;
class LetExpression;
class PiExpression;

class ExpressionVisitor {
 public:
  virtual void visit_application(const ApplicationExpression &e) = 0;
  virtual void visit_id(const IdExpression &e) = 0;
  virtual void visit_lambda(const LambdaExpression &e) = 0;
  virtual void visit_let(const LetExpression &e) = 0;
  virtual void visit_pi(const PiExpression &e) = 0;
};

class MutatingExpressionVisitor {
 public:
  virtual void visit_application(ApplicationExpression &e) = 0;
  virtual void visit_id(IdExpression &e) = 0;
  virtual void visit_lambda(LambdaExpression &e) = 0;
  virtual void visit_let(LetExpression &e) = 0;
  virtual void visit_pi(PiExpression &e) = 0;
};

} // namespace script3025

#endif
