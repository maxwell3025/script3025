#include "expression/visitors/partial_clone_visitor.hpp"
namespace script3025 {

void PartialCloneVisitor::visit_application(const ApplicationExpression &) {
  make_default_like<ApplicationExpression>();
}
void PartialCloneVisitor::visit_equality(const EqualityExpression &e) {
  make_default_like<EqualityExpression>();
}
void PartialCloneVisitor::visit_id(const IdExpression &e) {
  make_default_like<IdExpression>();
}
void PartialCloneVisitor::visit_induction_keyword(const InductionKeywordExpression &e) {
  make_default_like<InductionKeywordExpression>();
}
void PartialCloneVisitor::visit_lambda(const LambdaExpression &e) {
  make_default_like<LambdaExpression>();
}
void PartialCloneVisitor::visit_let(const LetExpression &e) {
  make_default_like<LetExpression>();
}
void PartialCloneVisitor::visit_nat_keyword(const NatKeywordExpression &e) {
  make_default_like<NatKeywordExpression>();
}
void PartialCloneVisitor::visit_nat_literal(const NatLiteralExpression &e) {
  make_default_like<NatLiteralExpression>();
}
void PartialCloneVisitor::visit_pi(const PiExpression &e) {
  make_default_like<PiExpression>();
}
void PartialCloneVisitor::visit_replace_keyword(const ReplaceKeywordExpression &e) {
  make_default_like<ReplaceKeywordExpression>();
}
void PartialCloneVisitor::visit_reflexive_keyword(const ReflexiveKeywordExpression &e) {
  make_default_like<ReflexiveKeywordExpression>();
}
void PartialCloneVisitor::visit_succ_keyword(const SuccKeywordExpression &e) {
  make_default_like<SuccKeywordExpression>();
}
void PartialCloneVisitor::visit_type_keyword(const TypeKeywordExpression &e) {
  make_default_like<TypeKeywordExpression>();
}

std::unique_ptr<Expression> PartialCloneVisitor::get() {
  return std::move(value);
}

template <typename ExpressionSubtype>
std::unique_ptr<ExpressionSubtype> PartialCloneVisitor::make_default_like() {
  value = std::make_unique<ExpressionSubtype>();
}

}  // namespace script3025
