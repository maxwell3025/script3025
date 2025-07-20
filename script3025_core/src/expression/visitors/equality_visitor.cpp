#include "expression/visitors/equality_visitor.hpp"

#include "expression/expression.hpp"

namespace script3025 {

EqualityVisitor::EqualityVisitor() : unequal(false) {}

void EqualityVisitor::visit_id(const IdExpression &rhs) {
  pointer_map[lhs] = &rhs;

  if (typeid(lhs) != typeid(IdExpression)) {
    unequal = true;
    return;
  }

  const IdExpression *casted_lhs = static_cast<const IdExpression *>(lhs);

  const Expression *translated_lhs_source = casted_lhs->source;
  if (pointer_map.find(translated_lhs_source) != pointer_map.end())
    translated_lhs_source = pointer_map[translated_lhs_source];

  if (translated_lhs_source != rhs.source) unequal = true;
}

void EqualityVisitor::visit_nat_literal(const NatLiteralExpression &rhs) {
  pointer_map[lhs] = &rhs;

  if (typeid(lhs) != typeid(NatLiteralExpression)) {
    unequal = true;
    return;
  }

  // TODO: Implement value comparison
}

void EqualityVisitor::visit_type_keyword(const TypeKeywordExpression &rhs) {
  pointer_map[lhs] = &rhs;

  if (typeid(lhs) != typeid(TypeKeywordExpression)) unequal = true;

  if (static_cast<const TypeKeywordExpression *>(lhs)->level != rhs.level)
    unequal = true;
}

void EqualityVisitor::visit_default(const Expression &rhs) {
  pointer_map[lhs] = &rhs;

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
  }
}

}  // namespace script3025