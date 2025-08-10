#ifndef SCRIPT3025_SCRIPT3025_CORE_EXPRESSION_VISITOR_HPP
#define SCRIPT3025_SCRIPT3025_CORE_EXPRESSION_VISITOR_HPP

#include <type_traits>

namespace script3025 {

class Expression;
class ApplicationExpression;
class EqualityExpression;
class IdExpression;
class InductionKeywordExpression;
class KeywordExpression;
class LambdaExpression;
class LetExpression;
class NatLiteralExpression;
class NatKeywordExpression;
class PiExpression;
class ReplaceKeywordExpression;
class ReflexiveKeywordExpression;
class SuccKeywordExpression;
class ScopeExpression;
class TypeKeywordExpression;

template <const bool is_const>
class ExpressionVisitor {
 public:
  template <typename T>
  using ref = std::conditional_t<is_const, const T&, T&>;

  virtual ~ExpressionVisitor() = default;

#pragma region ScopeExpression
  virtual void visit_lambda(ref<LambdaExpression> e) { visit_scope(e); }
  virtual void visit_let(ref<LetExpression> e) { visit_scope(e); }
  virtual void visit_pi(ref<PiExpression> e) { visit_scope(e); }
#pragma endregion

#pragma region KeywordExpression
  virtual void visit_induction_keyword(ref<InductionKeywordExpression> e) {
    visit_keyword(e);
  }
  virtual void visit_nat_keyword(ref<NatKeywordExpression> e) {
    visit_keyword(e);
  }
  virtual void visit_replace_keyword(ref<ReplaceKeywordExpression> e) {
    visit_keyword(e);
  }
  virtual void visit_reflexive_keyword(ref<ReflexiveKeywordExpression> e) {
    visit_keyword(e);
  }
  virtual void visit_succ_keyword(ref<SuccKeywordExpression> e) {
    visit_keyword(e);
  }
  virtual void visit_type_keyword(ref<TypeKeywordExpression> e) {
    visit_keyword(e);
  }
#pragma endregion

  virtual void visit_application(ref<ApplicationExpression> e) {
    visit_expression(e);
  }
  virtual void visit_equality(ref<EqualityExpression> e) {
    visit_expression(e);
  }
  virtual void visit_id(ref<IdExpression> e) { visit_expression(e); }
  virtual void visit_nat_literal(ref<NatLiteralExpression> e) {
    visit_expression(e);
  }

  void visit(ref<Expression> e) { e.accept(*this); }

 protected:
  virtual void visit_expression(ref<Expression>) {}
  virtual void visit_keyword(ref<KeywordExpression> e) { visit_expression(e); }
  virtual void visit_scope(ref<ScopeExpression> e) { visit_expression(e); }
};

using ConstExpressionVisitor = ExpressionVisitor<true>;
using MutatingExpressionVisitor = ExpressionVisitor<false>;

}  // namespace script3025

#endif
