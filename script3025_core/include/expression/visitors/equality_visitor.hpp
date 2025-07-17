#ifndef SCRIPT3025_SCRIPT3025_CORE_EQUALITY_VISITOR_HPP
#define SCRIPT3025_SCRIPT3025_CORE_EQUALITY_VISITOR_HPP

#include "expression/expression_base.hpp"
#include "expression/expression_visitor.hpp"

namespace script3025 {

class EqualityVisitor : public ExpressionVisitor {
 public:
  EqualityVisitor();
  void visit_application(const ApplicationExpression &rhs) override;
  void visit_equality(const EqualityExpression &rhs) override;
  void visit_id(const IdExpression &rhs) override;
  void visit_induction_keyword(const InductionKeywordExpression &rhs) override;
  void visit_lambda(const LambdaExpression &rhs) override;
  void visit_let(const LetExpression &rhs) override;
  void visit_nat_keyword(const NatKeywordExpression &rhs) override;
  void visit_nat_literal(const NatLiteralExpression &rhs) override;
  void visit_pi(const PiExpression &rhs) override;
  void visit_replace_keyword(const ReplaceKeywordExpression &rhs) override;
  void visit_succ_keyword(const SuccKeywordExpression &e) override;
  void visit_type_keyword(const TypeKeywordExpression &rhs) override;

  bool unequal;
  const Expression *lhs;
  std::unordered_map<const Expression *, const Expression *> pointer_map;
};

} // namespace script3025

#endif
