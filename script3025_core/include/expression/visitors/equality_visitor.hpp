#ifndef SCRIPT3025_SCRIPT3025_CORE_EQUALITY_VISITOR_HPP
#define SCRIPT3025_SCRIPT3025_CORE_EQUALITY_VISITOR_HPP

#include "expression/expression_base.hpp"
#include "expression/expression_visitor.hpp"

namespace script3025 {

class EqualityVisitor : public ExpressionVisitor {
 public:
  EqualityVisitor(const Expression *lhs);
  void visit_id(const IdExpression &rhs) override;
  void visit_nat_literal(const NatLiteralExpression &rhs) override;
  void visit_type_keyword(const TypeKeywordExpression &rhs) override;
  bool get() const;

 protected:
  void visit_default(const Expression &e) override;

 private:
  bool unequal;
  const Expression *lhs;
  std::unordered_map<const Expression *, const Expression *> pointer_map;
};

}  // namespace script3025

#endif
