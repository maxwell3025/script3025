#include "expression/visitors/equality_visitor.hpp"

#include "expression/expression.hpp"

namespace script3025 {

EqualityVisitor::EqualityVisitor(const Expression *lhs)
    : unequal(false), lhs(lhs) {}

void EqualityVisitor::visit_id(const IdExpression &rhs) {
  pointer_map_[lhs] = &rhs;

  if (typeid(lhs) != typeid(IdExpression)) {
    unequal = true;
    return;
  }

  const IdExpression *casted_lhs = static_cast<const IdExpression *>(lhs);

  const Expression *translated_lhs_source = casted_lhs->source;
  if (pointer_map_.find(translated_lhs_source) != pointer_map_.end())
    translated_lhs_source = pointer_map_[translated_lhs_source];

  if (translated_lhs_source != rhs.source) unequal = true;
}

void EqualityVisitor::visit_nat_literal(const NatLiteralExpression &rhs) {
  visit_expression(rhs);
  if (static_cast<const NatLiteralExpression *>(lhs)->value_ != rhs.value_)
    unequal = true;
}

void EqualityVisitor::visit_type_keyword(const TypeKeywordExpression &rhs) {
  visit_expression(rhs);
  if (static_cast<const TypeKeywordExpression *>(lhs)->level_ != rhs.level_)
    unequal = true;
}

void EqualityVisitor::visit_expression(const Expression &rhs) {
  pointer_map_[lhs] = &rhs;

  if (typeid(lhs) != typeid(rhs)) {
    unequal = true;
    return;
  }

  for (size_t i = 0; i < rhs.children.size(); ++i) {
    if (rhs.children[i]) {
      lhs = lhs->children[i].get();
      visit(*rhs.children[i]);
    } else {
      unequal |= static_cast<bool>(lhs->children[i]);
    }
    if (unequal) return;
  }
}

bool EqualityVisitor::get() const { return !unequal; }

}  // namespace script3025