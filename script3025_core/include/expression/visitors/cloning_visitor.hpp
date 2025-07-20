#ifndef SCRIPT3025_SCRIPT3025_CORE_CLONING_VISITOR_HPP
#define SCRIPT3025_SCRIPT3025_CORE_CLONING_VISITOR_HPP

#include "expression/expression_base.hpp"
#include "expression/expression_visitor.hpp"

namespace script3025 {

class CloningVisitor : public ExpressionVisitor {
 public:
  void visit_id(const IdExpression &e) override;
  void visit_lambda(const LambdaExpression &e) override;
  void visit_let(const LetExpression &e) override;
  void visit_nat_literal(const NatLiteralExpression &e) override;
  void visit_pi(const PiExpression &e) override;
  void visit_type_keyword(const TypeKeywordExpression &e) override;
  std::unique_ptr<Expression> get();

  std::unique_ptr<Expression> value;
  std::unordered_map<const Expression *, Expression *> pointer_map;

 protected:
  void visit_default(const Expression &e) override;

 private:
  template <typename ExpressionSubtype>
  std::unique_ptr<ExpressionSubtype> clone(const Expression &e);
};

} // namespace script3025

#endif
