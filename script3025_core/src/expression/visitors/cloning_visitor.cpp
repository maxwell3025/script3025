#include "expression/visitors/cloning_visitor.hpp"

#include <cstddef>
#include <memory>
#include <utility>

#include "expression/expression.hpp"
#include "expression/subtypes/scope_expression.hpp"
#include "expression/visitors/partial_clone_visitor.hpp"

namespace script3025 {

void CloningVisitor::visit_id(const IdExpression &e) {
  std::unique_ptr<IdExpression> new_expression =
      std::make_unique<IdExpression>();

  new_expression->id = e.id;

  if (scope_expression_map_.find(e.source) != scope_expression_map_.end()) {
    new_expression->source = scope_expression_map_[e.source];
  } else {
    new_expression->source = e.source;
  }

  value_ = std::move(new_expression);
}

void CloningVisitor::visit_scope(const ScopeExpression &e) {
  std::unique_ptr<Expression> new_expression = make_default_like(e);

  scope_expression_map_[&e] =
      static_cast<ScopeExpression *>(new_expression.get());

  for (size_t i = 0; i < e.children.size(); ++i) {
    if (e.children[i])
      visit(*e.children[i]);
    else
      value_ = nullptr;
    new_expression->children[i] = get();
  }

  value_ = std::move(new_expression);
  ScopeExpression &casted_expression = static_cast<ScopeExpression &>(*value_);
  casted_expression.argument_id = e.argument_id;
}

void CloningVisitor::visit_nat_literal(const NatLiteralExpression &e) {
  visit_expression(e);
  NatLiteralExpression &casted_expression =
      static_cast<NatLiteralExpression &>(*value_);
  casted_expression.value = e.value;
}

void CloningVisitor::visit_type_keyword(const TypeKeywordExpression &e) {
  visit_expression(e);
  TypeKeywordExpression &casted_expression =
      static_cast<TypeKeywordExpression &>(*value_);
  casted_expression.level = e.level;
}

void CloningVisitor::visit_expression(const Expression &e) {
  std::unique_ptr<Expression> new_expression = make_default_like(e);

  for (size_t i = 0; i < e.children.size(); ++i) {
    if (e.children[i])
      visit(*e.children[i]);
    else
      value_ = nullptr;
    new_expression->children[i] = get();
  }

  value_ = std::move(new_expression);
}

std::unique_ptr<Expression> CloningVisitor::get() { return std::move(value_); }

}  // namespace script3025
