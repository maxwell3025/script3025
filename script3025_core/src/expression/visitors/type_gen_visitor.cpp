#include "expression/visitors/type_gen_visitor.hpp"

#include <memory>

#include "expression/expression.hpp"
#include "expression/subtypes/application_expression.hpp"
#include "expression/subtypes/pi_expression.hpp"
#include "expression/subtypes/scope_expression.hpp"
#include "expression/subtypes/type_keyword_expression.hpp"
#include "spdlog/spdlog.h"

namespace script3025 {

void TypeGenVisitor::visit_expression(const Expression &e) {
  for (const std::unique_ptr<Expression> &child : e.children) {
    visit(*child);
  }
}

void TypeGenVisitor::visit_induction_keyword(
    const InductionKeywordExpression &e) {
  // The type of the motive is Pi (input : Nat). Type
  // TODO add universe polymorphism here
  std::unique_ptr<Expression> motive_type = std::make_unique<PiExpression>(
      "input", std::make_unique<NatLiteralExpression>(),
      std::make_unique<TypeKeywordExpression>());

  // Destructors take the destruction results of n and produce the destruction
  // result of succ n. The type is Pi (n: Nat). Pi (prev: motive n). motive
  // (succ n)
  std::unique_ptr<Expression> destructor_type =
      std::make_unique<PiExpression>();

  std::unique_ptr<Expression> base_type =
      std::make_unique<ApplicationExpression>();

  std::unique_ptr<Expression> input_type =
      std::make_unique<NatLiteralExpression>();

  std::unique_ptr<Expression> output_type =
      std::make_unique<ApplicationExpression>();

  std::unique_ptr<PiExpression> input_scope = std::make_unique<PiExpression>(
      "input", std::move(input_type), std::move(output_type));
  std::unique_ptr<ScopeExpression> base_scope = std::make_unique<PiExpression>(
      "base", std::move(base_type), std::move(input_scope));
  std::unique_ptr<ScopeExpression> destructor_scope =
      std::make_unique<PiExpression>("destructor", std::move(destructor_type),
                                     std::move(base_scope));
  std::unique_ptr<ScopeExpression> motive_scope =
      std::make_unique<PiExpression>("motive", std::move(motive_type),
                                     std::move(destructor_scope));

  expression_type_map_[&e] = std::move(motive_scope);
}
void TypeGenVisitor::visit_nat_keyword(const NatKeywordExpression &e) {
  expression_type_map_[&e] = std::make_unique<TypeKeywordExpression>();
}

void TypeGenVisitor::visit_replace_keyword(const ReplaceKeywordExpression &) {
  // TODO big complex expression.
}

void TypeGenVisitor::visit_reflexive_keyword(
    const ReflexiveKeywordExpression &) {
  // TODO big complex expression.
}

void TypeGenVisitor::visit_succ_keyword(const SuccKeywordExpression &e) {
  expression_type_map_[&e] = std::make_unique<PiExpression>(
      "n", std::make_unique<NatLiteralExpression>(),
      std::make_unique<NatLiteralExpression>());
}

void TypeGenVisitor::visit_type_keyword(const TypeKeywordExpression &e) {
  expression_type_map_[&e] =
      std::make_unique<TypeKeywordExpression>(e.level + 1);
}

void TypeGenVisitor::visit_application(const ApplicationExpression &e) {
  // Assert that the type of the function is a Pi type.
}

void TypeGenVisitor::visit_pi(const PiExpression &e) {
  expression_type_map_[&e] = std::make_unique<TypeKeywordExpression>(
      /* TODO decide universe level rules */
  );
}

void TypeGenVisitor::visit_equality(const EqualityExpression &) {
  // TODO universe rules.
}
void TypeGenVisitor::visit_id(const IdExpression &e) {
  if (variable_type_map_.find(e.get_variable_reference()) ==
      variable_type_map_.end()) {
    SPDLOG_LOGGER_ERROR(get_logger(),
                        "Error: no type for variable {} in type generation", e);
  }
  expression_type_map_[&e] =
      variable_type_map_[e.get_variable_reference()]->clone();
}
void TypeGenVisitor::visit_nat_literal(const NatLiteralExpression &) {}

}  // namespace script3025