#ifndef SCRIPT3025_CORE_EXPRESSION_VISITORS_PARTIAL_CLONE_VISITOR_HPP
#define SCRIPT3025_CORE_EXPRESSION_VISITORS_PARTIAL_CLONE_VISITOR_HPP

#include "expression/expression.hpp"

namespace script3025 {

class PartialCloneVisitor : public ConstExpressionVisitor {
 public:
  void visit_application(const ApplicationExpression &e) override {
    make_default_like<ApplicationExpression>();
  }
  void visit_equality(const EqualityExpression &e) override {
    make_default_like<EqualityExpression>();
  }
  void visit_id(const IdExpression &e) override {
    make_default_like<IdExpression>();
  }
  void visit_induction_keyword(const InductionKeywordExpression &e) override {
    make_default_like<InductionKeywordExpression>();
  }
  void visit_lambda(const LambdaExpression &e) override {
    make_default_like<LambdaExpression>();
  }
  void visit_let(const LetExpression &e) override {
    make_default_like<LetExpression>();
  }
  void visit_nat_keyword(const NatKeywordExpression &e) override {
    make_default_like<NatKeywordExpression>();
  }
  void visit_nat_literal(const NatLiteralExpression &e) override {
    make_default_like<NatLiteralExpression>();
  }
  void visit_pi(const PiExpression &e) override {
    make_default_like<PiExpression>();
  }
  void visit_reflexive_keyword(const ReflexiveKeywordExpression &e) override {
    make_default_like<ReflexiveKeywordExpression>();
  }
  void visit_replace_keyword(const ReplaceKeywordExpression &e) override {
    make_default_like<ReplaceKeywordExpression>();
  }
  void visit_succ_keyword(const SuccKeywordExpression &e) override {
    make_default_like<SuccKeywordExpression>();
  }
  void visit_type_keyword(const TypeKeywordExpression &e) override {
    make_default_like<TypeKeywordExpression>();
  }
  [[nodiscard]] std::unique_ptr<Expression> get() { return std::move(value_); };

 private:
  template <typename ExpressionSubtype>
  void make_default_like() {
    value_ = std::make_unique<ExpressionSubtype>();
  }

  std::unique_ptr<Expression> value_;
};

[[nodiscard]] inline std::unique_ptr<Expression> make_default_like(const Expression &e) {
  PartialCloneVisitor visitor;
  visitor.visit(e);
  return visitor.get();
}

}  // namespace script3025

#endif
