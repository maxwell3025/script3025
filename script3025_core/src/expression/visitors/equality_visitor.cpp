#include "expression/visitors/equality_visitor.hpp"

#include <cstddef>

#include "expression/expression.hpp"

namespace script3025 {

EqualityVisitor::EqualityVisitor(const Expression *lhs)
    : unequal_(false), lhs_(lhs) {}

void EqualityVisitor::visit_id(const IdExpression &rhs) {
  pointer_map_[lhs_] = &rhs;

  if (typeid(lhs_) != typeid(IdExpression)) {
    unequal_ = true;
    return;
  }

  const IdExpression *casted_lhs = static_cast<const IdExpression *>(lhs_);

  const Expression *translated_lhs_source = casted_lhs->source;
  if (pointer_map_.find(translated_lhs_source) != pointer_map_.end())
    translated_lhs_source = pointer_map_[translated_lhs_source];

  if (translated_lhs_source != rhs.source) unequal_ = true;
}

void EqualityVisitor::visit_nat_literal(const NatLiteralExpression &rhs) {
  visit_expression(rhs);
  if (static_cast<const NatLiteralExpression *>(lhs_)->value != rhs.value)
    unequal_ = true;
}

void EqualityVisitor::visit_type_keyword(const TypeKeywordExpression &rhs) {
  visit_expression(rhs);
  if (static_cast<const TypeKeywordExpression *>(lhs_)->level != rhs.level)
    unequal_ = true;
}

void EqualityVisitor::visit_expression(const Expression &rhs) {
  pointer_map_[lhs_] = &rhs;

  if (typeid(lhs_) != typeid(rhs)) {
    unequal_ = true;
    return;
  }

  for (size_t i = 0; i < rhs.children.size(); ++i) {
    if (rhs.children[i]) {
      lhs_ = lhs_->children[i].get();
      visit(*rhs.children[i]);
    } else {
      unequal_ |= static_cast<bool>(lhs_->children[i]);
    }
    if (unequal_) return;
  }
}

bool EqualityVisitor::get() const { return !unequal_; }

}  // namespace script3025