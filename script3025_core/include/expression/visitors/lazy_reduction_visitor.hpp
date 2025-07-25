#ifndef SCRIPT3025_SCRIPT3025_CORE_LAZY_REDUCTION_VISITOR
#define SCRIPT3025_SCRIPT3025_CORE_LAZY_REDUCTION_VISITOR

#include "expression/expression_base.hpp"
#include "expression/expression_visitor.hpp"

namespace script3025 {

class LazyReductionVisitor : public MutatingExpressionVisitor {
 public:
  void visit_application(ApplicationExpression &e) override;
  void visit_equality(EqualityExpression &e) override;
  void visit_id(IdExpression &e) override;
  void visit_induction_keyword(InductionKeywordExpression &e) override;
  void visit_lambda(LambdaExpression &e) override;
  void visit_let(LetExpression &e) override;
  void visit_nat_keyword(NatKeywordExpression &e) override;
  void visit_nat_literal(NatLiteralExpression &e) override;
  void visit_pi(PiExpression &e) override;
  void visit_replace_keyword(ReplaceKeywordExpression &e) override;
  void visit_succ_keyword(SuccKeywordExpression &e) override;
  void visit_type_keyword(TypeKeywordExpression &e) override;
  
  std::unique_ptr<Expression> reduced_expression;

 protected:
  void visit_expression(Expression &e);

 private:
  static std::shared_ptr<spdlog::logger> get_logger();
};

} // namespace script3025

#endif
