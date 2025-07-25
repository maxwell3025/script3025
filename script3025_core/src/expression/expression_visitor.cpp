#include "expression/expression_visitor.hpp"

#include "expression/expression.hpp"

namespace script3025 {

void ExpressionVisitor::visit_expression(const Expression &e) {}
void MutatingExpressionVisitor::visit_expression(Expression &e) {}

#pragma region ScopeExpression

void ExpressionVisitor::visit_scope(const ScopeExpression &e) {
  visit_expression(e);
}
void MutatingExpressionVisitor::visit_scope(ScopeExpression &e) {
  visit_expression(e);
}
void MutatingExpressionVisitor::visit_pi(PiExpression &e) { visit_scope(e); }
void ExpressionVisitor::visit_lambda(const LambdaExpression &e) {
  visit_scope(e);
}
void MutatingExpressionVisitor::visit_lambda(LambdaExpression &e) {
  visit_scope(e);
}
void ExpressionVisitor::visit_let(const LetExpression &e) { visit_scope(e); }
void MutatingExpressionVisitor::visit_let(LetExpression &e) { visit_scope(e); }
void ExpressionVisitor::visit_pi(const PiExpression &e) { visit_scope(e); }

#pragma endregion

#pragma region KeywordExpression

void ExpressionVisitor::visit_keyword(const KeywordExpression &e) {
  visit_expression(e);
}
void MutatingExpressionVisitor::visit_keyword(KeywordExpression &e) {
  visit_expression(e);
}
void ExpressionVisitor::visit_induction_keyword(
    const InductionKeywordExpression &e) {
  visit_keyword(e);
}
void MutatingExpressionVisitor::visit_induction_keyword(
    InductionKeywordExpression &e) {
  visit_keyword(e);
}
void ExpressionVisitor::visit_nat_keyword(const NatKeywordExpression &e) {
  visit_keyword(e);
}
void MutatingExpressionVisitor::visit_nat_keyword(NatKeywordExpression &e) {
  visit_keyword(e);
}
void ExpressionVisitor::visit_replace_keyword(
    const ReplaceKeywordExpression &e) {
  visit_keyword(e);
}
void MutatingExpressionVisitor::visit_replace_keyword(
    ReplaceKeywordExpression &e) {
  visit_keyword(e);
}
void ExpressionVisitor::visit_reflexive_keyword(
    const ReflexiveKeywordExpression &e) {
  visit_keyword(e);
}
void MutatingExpressionVisitor::visit_reflexive_keyword(
    ReflexiveKeywordExpression &e) {
  visit_keyword(e);
}
void ExpressionVisitor::visit_succ_keyword(const SuccKeywordExpression &e) {
  visit_keyword(e);
}
void MutatingExpressionVisitor::visit_succ_keyword(SuccKeywordExpression &e) {
  visit_keyword(e);
}
void ExpressionVisitor::visit_type_keyword(const TypeKeywordExpression &e) {
  visit_keyword(e);
}
void MutatingExpressionVisitor::visit_type_keyword(TypeKeywordExpression &e) {
  visit_keyword(e);
}

#pragma endregion

void ExpressionVisitor::visit_application(const ApplicationExpression &e) {
  visit_expression(e);
}
void MutatingExpressionVisitor::visit_application(ApplicationExpression &e) {
  visit_expression(e);
}
void ExpressionVisitor::visit_equality(const EqualityExpression &e) {
  visit_expression(e);
}
void MutatingExpressionVisitor::visit_equality(EqualityExpression &e) {
  visit_expression(e);
}
void ExpressionVisitor::visit_id(const IdExpression &e) { visit_expression(e); }
void MutatingExpressionVisitor::visit_id(IdExpression &e) {
  visit_expression(e);
}
void ExpressionVisitor::visit_nat_literal(const NatLiteralExpression &e) {
  visit_expression(e);
}
void MutatingExpressionVisitor::visit_nat_literal(NatLiteralExpression &e) {
  visit_expression(e);
}
void ExpressionVisitor::visit(const Expression &e) { e.accept(*this); }
void MutatingExpressionVisitor::visit(Expression &e) { e.accept(*this); }

}  // namespace script3025
