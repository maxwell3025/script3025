#include "expression/visitors/cloning_visitor.hpp"

#include "expression/expression.hpp"
#include "expression/visitors/partial_clone_visitor.hpp"

namespace script3025 {

void CloningVisitor::visit_id(const IdExpression &e) {
  std::unique_ptr<IdExpression> new_expression =
      std::make_unique<IdExpression>();
  pointer_map[&e] = new_expression.get();

  new_expression -> id = e.id;

  if (pointer_map.find(e.source) != pointer_map.end()) {
    new_expression -> source = pointer_map[e.source];
  } else {
    new_expression -> source = e.source;
  }

  value = std::move(new_expression);
}

void CloningVisitor::visit_lambda(const LambdaExpression &e) {
  visit_default(e);
  LambdaExpression &casted_expression = static_cast<LambdaExpression &>(*value);
  casted_expression.argument_id = e.argument_id;
}

void CloningVisitor::visit_let(const LetExpression &e) {
  visit_default(e);
  LetExpression &casted_expression = static_cast<LetExpression &>(*value);
  casted_expression.argument_id = e.argument_id;
}

void CloningVisitor::visit_nat_literal(const NatLiteralExpression &e) {
  visit_default(e);
  NatLiteralExpression &casted_expression =
      static_cast<NatLiteralExpression &>(*value);
  // TODO implement special handling
}

void CloningVisitor::visit_pi(const PiExpression &e) {
  visit_default(e);
  PiExpression &casted_expression = static_cast<PiExpression &>(*value);
  casted_expression.argument_id = e.argument_id;
}

void CloningVisitor::visit_type_keyword(const TypeKeywordExpression &e) {
  visit_default(e);
  TypeKeywordExpression &casted_expression =
      static_cast<TypeKeywordExpression &>(*value);
  casted_expression.level = e.level;
}

std::unique_ptr<Expression> CloningVisitor::get() {
  return std::move(value);
}

void CloningVisitor::visit_default(const Expression &e) {
  std::unique_ptr<Expression> new_expression = make_default_like(e);

  pointer_map[&e] = new_expression.get();

  for (size_t i = 0; i < e.children.size(); ++i) {
    if (e.children[i]) visit(*e.children[i]);
    else value = nullptr;
    new_expression -> children[i] = std::move(get());
  }

  value = std::move(new_expression);
}

} // namespace script3025
