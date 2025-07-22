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
class ReflexiveKeywordExpression;
class SuccKeywordExpression;
class TypeKeywordExpression;

class ExpressionVisitor {
 public:
  virtual void visit_application(const ApplicationExpression &e);
  virtual void visit_equality(const EqualityExpression &e);
  virtual void visit_id(const IdExpression &e);
  virtual void visit_induction_keyword(const InductionKeywordExpression &e);
  virtual void visit_lambda(const LambdaExpression &e);
  virtual void visit_let(const LetExpression &e);
  virtual void visit_nat_keyword(const NatKeywordExpression &e);
  virtual void visit_nat_literal(const NatLiteralExpression &e);
  virtual void visit_pi(const PiExpression &e);
  virtual void visit_replace_keyword(const ReplaceKeywordExpression &e);
  virtual void visit_reflexive_keyword(const ReflexiveKeywordExpression &e);
  virtual void visit_succ_keyword(const SuccKeywordExpression &e);
  virtual void visit_type_keyword(const TypeKeywordExpression &e);
  void visit(const Expression &e);

 protected:
  virtual void visit_default(const Expression &e);
};

class MutatingExpressionVisitor {
 public:
  virtual void visit_application(ApplicationExpression &e);
  virtual void visit_equality(EqualityExpression &e);
  virtual void visit_id(IdExpression &e);
  virtual void visit_induction_keyword(InductionKeywordExpression &e);
  virtual void visit_lambda(LambdaExpression &e);
  virtual void visit_let(LetExpression &e);
  virtual void visit_nat_keyword(NatKeywordExpression &e);
  virtual void visit_nat_literal(NatLiteralExpression &e);
  virtual void visit_pi(PiExpression &e);
  virtual void visit_replace_keyword(ReplaceKeywordExpression &e);
  virtual void visit_reflexive_keyword(ReflexiveKeywordExpression &e);
  virtual void visit_succ_keyword(SuccKeywordExpression &e);
  virtual void visit_type_keyword(TypeKeywordExpression &e);
  void visit(Expression &e);

 protected:
  virtual void visit_default(Expression &e);
};

} // namespace script3025

#endif
