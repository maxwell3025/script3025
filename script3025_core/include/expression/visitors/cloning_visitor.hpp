#ifndef SCRIPT3025_SCRIPT3025_CORE_CLONING_VISITOR_HPP
#define SCRIPT3025_SCRIPT3025_CORE_CLONING_VISITOR_HPP

#include "expression/expression_base.hpp"
#include "expression/expression_visitor.hpp"

namespace script3025 {

class CloningVisitor : public ConstExpressionVisitor {
 public:
  void visit_id(const IdExpression &e) override;
  void visit_scope(const ScopeExpression &e) override;
  void visit_nat_literal(const NatLiteralExpression &e) override;
  void visit_type_keyword(const TypeKeywordExpression &e) override;
  [[nodiscard]] std::unique_ptr<Expression> get();

 protected:
  void visit_expression(const Expression &e) override;

  std::unique_ptr<Expression> value_;
  std::unordered_map<const Expression *, Expression *> pointer_map_;
};

} // namespace script3025

#endif
