#ifndef SCRIPT3025_SCRIPT3025_CORE_EXPRESSION_VISITOR_HPP
#define SCRIPT3025_SCRIPT3025_CORE_EXPRESSION_VISITOR_HPP

namespace script3025 {

class Expression;
class ApplicationExpression;
class EqualityExpression;
class IdExpression;
class InductionKeywordExpression;
class LambdaExpression;
class LetExpression;
class NatLiteralExpression;
class NatKeywordExpression;
class PiExpression;
class ReplaceKeywordExpression;
class TypeKeywordExpression;

class ExpressionVisitor {
 public:
  virtual void visit_application(const ApplicationExpression &e) = 0;
  virtual void visit_equality(const EqualityExpression &e) = 0;
  virtual void visit_id(const IdExpression &e) = 0;
  virtual void visit_induction_keyword(const InductionKeywordExpression &e) = 0;
  virtual void visit_lambda(const LambdaExpression &e) = 0;
  virtual void visit_let(const LetExpression &e) = 0;
  virtual void visit_nat_keyword(const NatKeywordExpression &e) = 0;
  virtual void visit_nat_literal(const NatLiteralExpression &e) = 0;
  virtual void visit_pi(const PiExpression &e) = 0;
  virtual void visit_replace_keyword(const ReplaceKeywordExpression &e) = 0;
  virtual void visit_type_keyword(const TypeKeywordExpression &e) = 0;
  void visit(const Expression &e);
};

class MutatingExpressionVisitor {
 public:
  virtual void visit_application(ApplicationExpression &e) = 0;
  virtual void visit_equality(EqualityExpression &e) = 0;
  virtual void visit_id(IdExpression &e) = 0;
  virtual void visit_induction_keyword(InductionKeywordExpression &e) = 0;
  virtual void visit_lambda(LambdaExpression &e) = 0;
  virtual void visit_let(LetExpression &e) = 0;
  virtual void visit_nat_keyword(NatKeywordExpression &e) = 0;
  virtual void visit_nat_literal(NatLiteralExpression &e) = 0;
  virtual void visit_pi(PiExpression &e) = 0;
  virtual void visit_replace_keyword(ReplaceKeywordExpression &e) = 0;
  virtual void visit_type_keyword(TypeKeywordExpression &e) = 0;
  void visit(Expression &e);
};

} // namespace script3025

#endif
