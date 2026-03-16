#include "expression/visitors/type_gen_visitor.hpp"

#include <memory>

#include "expression/expression.hpp"
#include "expression/subtypes/lambda_expression.hpp"
#include "expression/subtypes/nat_keyword_expression.hpp"
#include "expression/subtypes/pi_expression.hpp"
#include "expression/variable_reference.hpp"
#include "expression/visitors/replacing_visitor.hpp"
#include "expression_factory.hpp"
#include "spdlog/spdlog.h"

namespace script3025 {

TypeGenVisitor::TypeGenVisitor(
    std::unordered_map<const Expression *, std::unique_ptr<Expression>>
        expression_type_map,
    std::unordered_map<VariableReference, std::unique_ptr<Expression>>
        variable_type_map)
    : expression_type_map_(std::move(expression_type_map)),
      variable_type_map_(std::move(variable_type_map)) {}

void TypeGenVisitor::visit_lambda(const LambdaExpression &e) {
  VariableReference arg_ref{e.argument_id, const_cast<LambdaExpression *>(&e)};
  variable_type_map_[arg_ref] = e.argument_type()->clone();
  visit(*e.definition());
  expression_type_map_[&e] = std::make_unique<PiExpression>(
      e.argument_id, e.argument_type()->clone(),
      expression_type_map_[e.definition().get()]->clone());
  SPDLOG_LOGGER_TRACE(get_logger(), "The type of {}:\n{}", e.to_string(),
                      expression_type_map_[&e]->to_string());
}

void TypeGenVisitor::visit_let(const LetExpression &e) {}

void TypeGenVisitor::visit_pi(const PiExpression &e) {
  visit(*e.argument_type());
  visit(*e.definition());
  expression_type_map_[&e] = std::make_unique<TypeKeywordExpression>(
      /* TODO decide universe level rules */
  );
  SPDLOG_LOGGER_TRACE(get_logger(), "The type of {}:\n{}", e.to_string(),
                      expression_type_map_[&e]->to_string());
}

void TypeGenVisitor::visit_induction_keyword(
    const InductionKeywordExpression &e) {
  static const std::string inductive_type_str =
      ("Pi (motive : Pi (input : Nat). Type)."
       "Pi (destructor : Pi (n : Nat). Pi (prev : motive n). motive (succ n))."
       "Pi (base : motive 0)."
       "Pi (input : Nat)."
       "motive input");

  // TODO cache this and just copy for efficiency.
  expression_type_map_[&e] = text_to_expression(inductive_type_str);
  SPDLOG_LOGGER_TRACE(get_logger(), "The type of {}:\n{}", e.to_string(),
                      expression_type_map_[&e]->to_string());
}

void TypeGenVisitor::visit_nat_keyword(const NatKeywordExpression &e) {
  expression_type_map_[&e] = std::make_unique<TypeKeywordExpression>();
  SPDLOG_LOGGER_TRACE(get_logger(), "The type of {}:\n{}", e.to_string(),
                      expression_type_map_[&e]->to_string());
}

void TypeGenVisitor::visit_replace_keyword(const ReplaceKeywordExpression &e) {
  // TODO big complex expression.
}

void TypeGenVisitor::visit_reflexive_keyword(
    const ReflexiveKeywordExpression &e) {
  // TODO big complex expression.
}

void TypeGenVisitor::visit_succ_keyword(const SuccKeywordExpression &e) {
  expression_type_map_[&e] = std::make_unique<PiExpression>(
      "n", std::make_unique<NatKeywordExpression>(),
      std::make_unique<NatKeywordExpression>());
  SPDLOG_LOGGER_TRACE(get_logger(), "The type of {}:\n{}", e.to_string(),
                      expression_type_map_[&e]->to_string());
}

void TypeGenVisitor::visit_type_keyword(const TypeKeywordExpression &e) {
  expression_type_map_[&e] =
      std::make_unique<TypeKeywordExpression>(e.level + 1);
  SPDLOG_LOGGER_TRACE(get_logger(), "The type of {}:\n{}", e.to_string(),
                      expression_type_map_[&e]->to_string());
}

void TypeGenVisitor::visit_application(const ApplicationExpression &e) {
  // Assert that the type of the function is a Pi type.
  visit(*e.function());
  visit(*e.argument());
  std::unique_ptr<Expression> function_type =
      expression_type_map_[e.function().get()]->clone();
  // Here, we are intentionally doing a RTTI lookup to get the runtime type of
  // function_type. We want side-effects to be evaluated, hence the lint ignore.
  // NOLINTNEXTLINE
  if (typeid(*function_type) != typeid(PiExpression)) {
    SPDLOG_LOGGER_ERROR(get_logger(),
                        "Error: expected Pi type in application, got {}",
                        function_type->to_string());
    return;
  }

  PiExpression &casted_function_type =
      static_cast<PiExpression &>(*function_type);

  ReplacingVisitor replacer(&casted_function_type,
                            casted_function_type.argument_id,
                            e.argument().get());
  replacer.visit(*casted_function_type.definition());

  std::unique_ptr<Expression> application_type = replacer.get();

  expression_type_map_[&e] = std::move(application_type);
  SPDLOG_LOGGER_TRACE(get_logger(), "The type of {}:\n{}", e.to_string(),
                      expression_type_map_[&e]->to_string());
}

void TypeGenVisitor::visit_equality(const EqualityExpression &e) {
  // TODO universe rules.
}
void TypeGenVisitor::visit_id(const IdExpression &e) {
  if (variable_type_map_.find(e.get_variable_reference()) ==
      variable_type_map_.end()) {
    SPDLOG_LOGGER_ERROR(get_logger(),
                        "Error: no type for variable {} in type generation",
                        e.id);
  }
  expression_type_map_[&e] =
      variable_type_map_[e.get_variable_reference()]->clone();
  SPDLOG_LOGGER_TRACE(get_logger(), "The type of {}:\n{}", e.to_string(),
                      expression_type_map_[&e]->to_string());
}
void TypeGenVisitor::visit_nat_literal(const NatLiteralExpression &e) {
  expression_type_map_[&e] = std::make_unique<NatKeywordExpression>();
  SPDLOG_LOGGER_TRACE(get_logger(), "The type of {}:\n{}", e.to_string(),
                      expression_type_map_[&e]->to_string());
}

}  // namespace script3025