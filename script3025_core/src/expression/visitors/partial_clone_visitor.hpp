#ifndef SCRIPT3025_CORE_EXPRESSION_VISITORS_PARTIAL_CLONE_VISITOR_HPP
#define SCRIPT3025_CORE_EXPRESSION_VISITORS_PARTIAL_CLONE_VISITOR_HPP

#include "expression/expression.hpp"

namespace script3025 {

class PartialCloneVisitor : public ExpressionVisitor {
 public:
  void visit_application(const ApplicationExpression &e) override;
  void visit_equality(const EqualityExpression &e) override;
  void visit_id(const IdExpression &e) override;
  void visit_induction_keyword(const InductionKeywordExpression &e) override;
  void visit_lambda(const LambdaExpression &e) override;
  void visit_let(const LetExpression &e) override;
  void visit_nat_keyword(const NatKeywordExpression &e) override;
  void visit_nat_literal(const NatLiteralExpression &e) override;
  void visit_pi(const PiExpression &e) override;
  void visit_replace_keyword(const ReplaceKeywordExpression &e) override;
  void visit_succ_keyword(const SuccKeywordExpression &e) override;
  void visit_type_keyword(const TypeKeywordExpression &e) override;
  std::unique_ptr<Expression> get();

  std::unique_ptr<Expression> value;

 private:
  template <typename ExpressionSubtype>
  std::unique_ptr<ExpressionSubtype> make_default_like();
};

inline std::unique_ptr<Expression> make_default_like(const Expression &e) {
  PartialCloneVisitor visitor;
  visitor.visit(e);
  return visitor.get();
}

}  // namespace script3025

#endif
