#include "expression/visitors/type_gen_visitor.hpp"

#include <gmpxx.h>

#include <algorithm>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <utility>

#include "expression/expression.hpp"
#include "expression/subtypes/id_expression.hpp"
#include "expression/subtypes/lambda_expression.hpp"
#include "expression/subtypes/nat_keyword_expression.hpp"
#include "expression/subtypes/nat_literal_expression.hpp"
#include "expression/subtypes/pi_expression.hpp"
#include "expression/subtypes/type_keyword_expression.hpp"
#include "expression/variable_reference.hpp"
#include "expression/visitors/lazy_reduction_visitor.hpp"
#include "expression/visitors/replacing_visitor.hpp"
#include "expression_factory.hpp"
#include "spdlog/fmt/bundled/format.h"
#include "spdlog/spdlog.h"

namespace script3025 {

// TODO cache all of the text_to_expresion calls and just copy for efficiency.
TypeGenVisitor::TypeGenVisitor(
    std::unordered_map<const Expression *, std::unique_ptr<Expression>>
        expression_type_map,
    std::unordered_map<VariableReference, std::unique_ptr<Expression>>
        variable_type_map)
    : expression_type_map_(std::move(expression_type_map)),
      variable_type_map_(std::move(variable_type_map)) {}

void TypeGenVisitor::visit_lambda(const LambdaExpression &e) {
  generate_type(*e.argument_type());
  if (!has_type(e.argument_type().get())) return;
  variable_type_map_[{e.argument_id, const_cast<LambdaExpression *>(&e)}] =
      e.argument_type()->clone();
  generate_type(*e.definition());
  if (!has_type(e.definition().get())) return;

  std::unique_ptr<PiExpression> expression_type =
      std::make_unique<PiExpression>(e.argument_id, e.argument_type()->clone(),
                                     nullptr);
  variable_type_map_[{e.argument_id, expression_type.get()}] =
      e.argument_type()->clone();

  std::unique_ptr<Expression> const replacement =
      std::make_unique<IdExpression>(e.argument_id, expression_type.get());
  ReplacingVisitor replacer(const_cast<LambdaExpression *>(&e), e.argument_id,
                            replacement.get());
  replacer.visit(*expression_type_map_[e.definition().get()]);
  expression_type->definition() = replacer.get();

  expression_type_map_[&e] = std::move(expression_type);
}

void TypeGenVisitor::visit_let(const LetExpression &e) {
  // This one requires some care to avoid ininite let var = term in Type k for
  // all k loops. Notice that in
  // https://rocq-prover.org/doc/V9.1.1/refman/language/cic.html#id4, the let
  // rule forces substitutions.

  generate_type(*e.argument_type());
  generate_type(*e.argument_value());
  if (!has_type(e.argument_type().get())) return;
  if (!has_type(e.argument_value().get())) return;

  VariableReference expression_reference{e.argument_id,
                                         const_cast<LetExpression *>(&e)};
  variable_type_map_[expression_reference] = e.argument_type()->clone();

  if (delta_table_.find(expression_reference) != delta_table_.end()) {
    SPDLOG_LOGGER_ERROR(get_logger(),
                        "Error: duplicate let variable in delta table\n"
                        "Let expression: {}\n"
                        "Argument id: {}\n",
                        e.to_string(), e.argument_id);
    return;
  }
  delta_table_[expression_reference] = e.argument_value().get();

  generate_type(*e.definition());

  if (!has_type(e.definition().get())) return;

  std::unique_ptr inferred_argument_type = reduce_copy(
      *expression_type_map_[e.argument_value().get()], delta_table_);
  std::unique_ptr given_argument_type =
      reduce_copy(*e.argument_type(), delta_table_);

  // label of 0 means we infer
  if (*given_argument_type != *inferred_argument_type &&
      *given_argument_type != NatLiteralExpression(0)) {
    SPDLOG_LOGGER_ERROR(
        get_logger(),
        "Error: type of let argument value did not match declared type\n"
        "Let expression: {}\n"
        "Argument value: {}\n"
        "Type of argument value: {} (reduces to {})\n"
        "Declared argument type: {} (reduces to {})\n",
        e.to_string(), e.argument_value()->to_string(),
        expression_type_map_[e.argument_value().get()]->to_string(),
        inferred_argument_type->to_string(), e.argument_type()->to_string(),
        given_argument_type->to_string());
    return;
  }
  std::unique_ptr<Expression> type =
      expression_type_map_[e.definition().get()]->clone();
  ReplacingVisitor replacer(const_cast<LetExpression *>(&e), e.argument_id,
                            e.argument_value().get());
  replacer.visit(*type);
  type = replacer.get();
  expression_type_map_[&e] = std::move(type);
}

void TypeGenVisitor::visit_pi(const PiExpression &e) {
  generate_type(*e.argument_type());
  if (!has_type(e.argument_type().get())) return;
  variable_type_map_[{e.argument_id, const_cast<PiExpression *>(&e)}] =
      e.argument_type()->clone();
  generate_type(*e.definition());
  if (!has_type(e.definition().get())) return;

  auto reduced_argument_type =
      reduce_copy(*expression_type_map_[e.argument_type().get()], delta_table_);
  // Here, we are intentionally doing a RTTI lookup to get the runtime type of
  // `e.argument_type()`.
  // We want side-effects to be evaluated, hence the lint ignore.
  // NOLINTNEXTLINE
  if (typeid(*reduced_argument_type) !=
      typeid(TypeKeywordExpression)) {
    SPDLOG_LOGGER_ERROR(
        get_logger(),
        "Error: The type of the argument type in a Pi expression did not "
        "reduce to `Type n`.\n"
        "Pi expression: {}\n"
        "Argument type: {}\n"
        "Type of argument type: {}\n",
        e.to_string(), e.argument_type()->to_string(),
        reduced_argument_type->to_string());
    return;
  }

  auto reduced_definition_type =
      reduce_copy(*expression_type_map_[e.definition().get()], delta_table_);
  // Here, we are intentionally doing a RTTI lookup to get the runtime type of
  // `e.definition()`.
  // We want side-effects to be evaluated, hence the lint ignore.
  // NOLINTNEXTLINE
  if (typeid(*reduced_definition_type) !=
      typeid(TypeKeywordExpression)) {
    SPDLOG_LOGGER_ERROR(
        get_logger(),
        "Error: The type of the definition in a Pi expression did not "
        "reduce to `Type n`.\n"
        "Pi expression: {}\n"
        "Definition: {}\n"
        "Type of definition type: {}\n",
        e.to_string(), e.definition()->to_string(),
        reduced_definition_type->to_string());
    return;
  }

  mpz_class const argument_level =
      static_cast<TypeKeywordExpression *>(
          reduced_argument_type.get())
          ->level;
  mpz_class const definition_level =
      static_cast<TypeKeywordExpression *>(
          reduced_definition_type.get())
          ->level;
  // if (argument_level > definition_level) {
  //   SPDLOG_LOGGER_ERROR(
  //       get_logger(),
  //       "Error: Pi expression with higher-level argument than definition\n"
  //       "Pi expression: {}\n"
  //       "Type of Argument type: {}\n"
  //       "Definition type: {}\n",
  //       e.to_string(), e.argument_type()->to_string(),
  //       e.definition()->to_string());
  //   return;
  // }

  // EVIL!
  // expression_type_map_[&e] =
  //     std::make_unique<TypeKeywordExpression>(definition_level);

  // This is still predicative iirc
  auto expected_level = std::max(definition_level, argument_level);

  // impredicative prop
  if (definition_level == 0) {
    expected_level = 0;
  }

  expression_type_map_[&e] = std::make_unique<TypeKeywordExpression>(expected_level);
}

void TypeGenVisitor::visit_type_keyword(const TypeKeywordExpression &e) {
  expression_type_map_[&e] =
      std::make_unique<TypeKeywordExpression>(e.level + 1);
}

void TypeGenVisitor::visit_application(const ApplicationExpression &e) {
  // Assert that the type of the function is a Pi type.
  generate_type(*e.function());
  generate_type(*e.argument());
  if (!has_type(e.function().get()) || !has_type(e.argument().get())) return;

  std::unique_ptr<Expression> function_type =
      reduce_copy(*expression_type_map_[e.function().get()], delta_table_);

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

  std::unique_ptr<Expression> function_argument_type =
      reduce_copy(*casted_function_type.argument_type(), delta_table_);

  std::unique_ptr<Expression> argument_type =
      reduce_copy(*expression_type_map_[e.argument().get()], delta_table_);

  if (*function_argument_type != *argument_type) {
    SPDLOG_LOGGER_ERROR(get_logger(),
                        "Error: argument type mismatch in application\n"
                        "    Application expression: {}\n"
                        "    Expected argument type: {}\n"
                        "    Actual argument type: {}",
                        e.to_string(), function_argument_type->to_string(),
                        argument_type->to_string());
    return;
  }

  SPDLOG_LOGGER_TRACE(get_logger(),
                      "Computed function type:\n"
                      "    Application expression: {}\n"
                      "    Function type: {}\n"
                      "    Argument type: {}",
                      e.to_string(), function_type->to_string(),
                      argument_type->to_string());

  ReplacingVisitor replacer(&casted_function_type,
                            casted_function_type.argument_id,
                            e.argument().get());
  replacer.visit(*casted_function_type.definition());

  std::unique_ptr<Expression> application_type = replacer.get();

  expression_type_map_[&e] = std::move(application_type);
}

void TypeGenVisitor::visit_id(const IdExpression &e) {
  if (variable_type_map_.find(e.get_variable_reference()) ==
      variable_type_map_.end()) {
    SPDLOG_LOGGER_ERROR(get_logger(),
                        "Error: variable reference not found in variable type "
                        "map:\nid={}\nsource={}",
                        e.id, fmt::ptr(e.source));
    SPDLOG_LOGGER_TRACE(get_logger(), "Current variable type map:\n{}", [&]() {
      std::stringstream ss;
      for (const auto &[var_ref, type] : variable_type_map_) {
        ss << "Variable reference:\n"
           << "id=" << var_ref.id << "\n"
           << "source=" << fmt::ptr(var_ref.source) << "("
           << ((var_ref.source != nullptr) ? var_ref.source->to_string()
                                           : "null")
           << ")\n"
           << "Type:\n"
           << type->to_string() << "\n\n";
      }
      return ss.str();
    }());
    return;
  }
  expression_type_map_[&e] =
      variable_type_map_[e.get_variable_reference()]->clone();
}

void TypeGenVisitor::generate_type(const Expression &e) {
  SPDLOG_LOGGER_TRACE(get_logger(),
                      "Called generate_type.\n"
                      "    expr={} @ {}",
                      e.to_string(), fmt::ptr(&e));
  e.accept(*this);
  if (expression_type_map_.find(&e) == expression_type_map_.end()) {
    SPDLOG_LOGGER_ERROR(get_logger(),
                        "Error: no type generated for expression {}",
                        e.to_string());
    return;
  }
  SPDLOG_LOGGER_TRACE(get_logger(),
                      "Type of expression generated successfully.\n"
                      "    expr={} @ {}\n"
                      "    type={} @ {}",
                      e.to_string(), fmt::ptr(&e),
                      expression_type_map_[&e]->to_string(),
                      fmt::ptr(expression_type_map_[&e].get()));
  if (typeid(e) != typeid(TypeKeywordExpression)) {
    SPDLOG_LOGGER_TRACE(get_logger(),
                        "Type tower should be extended.\n"
                        "    expr={} @ {}\n"
                        "    type={} @ {}",
                        e.to_string(), fmt::ptr(&e),
                        expression_type_map_[&e]->to_string(),
                        fmt::ptr(expression_type_map_[&e].get()));
    generate_type(*expression_type_map_[&e]);
    if (expression_type_map_.find(expression_type_map_[&e].get()) ==
        expression_type_map_.end()) {
      SPDLOG_LOGGER_ERROR(get_logger(),
                          "Failed to generate type tower.\n"
                          "    expr={} @ {}\n"
                          "    type={} @ {}",
                          e.to_string(), fmt::ptr(&e),
                          expression_type_map_[&e]->to_string(),
                          fmt::ptr(expression_type_map_[&e].get()));
      expression_type_map_.erase(&e);
      return;
    }
    SPDLOG_LOGGER_TRACE(
        get_logger(),
        "Finished generating type tower.\n"
        "    expr={} @ {}\n"
        "    type={} @ {}\n"
        "    tptp={} @ {}",
        e.to_string(), fmt::ptr(&e), expression_type_map_[&e]->to_string(),
        fmt::ptr(expression_type_map_[&e].get()),
        expression_type_map_[expression_type_map_[&e].get()]->to_string(),
        fmt::ptr(expression_type_map_[expression_type_map_[&e].get()].get()));
  }
}

void TypeGenVisitor::visit_induction_keyword(
    const InductionKeywordExpression &e) {
  // This follows the recursor format described in
  // https://lean-lang.org/doc/reference/latest/The-Type-System/Inductive-Types/#recursor-types.

  static const std::string inductive_type_str =
      ("Pi (motive: Pi (input: Nat). Type)."
       "Pi (minor_premise_1: Pi (n: Nat). Pi (prev: motive n). motive (succ "
       "n))."
       "Pi (minor_premise_2: motive 0)."
       "Pi (major_premise: Nat)."
       "motive major_premise");
  expression_type_map_[&e] = text_to_expression(inductive_type_str);
}

void TypeGenVisitor::visit_nat_keyword(const NatKeywordExpression &e) {
  static const std::string nat_type_str = ("Type");
  expression_type_map_[&e] = text_to_expression(nat_type_str);
}

void TypeGenVisitor::visit_nat_literal(const NatLiteralExpression &e) {
  static const std::string nat_literal_type_str = ("Nat");
  expression_type_map_[&e] = text_to_expression(nat_literal_type_str);
}

void TypeGenVisitor::visit_replace_keyword(const ReplaceKeywordExpression &e) {
  // This follows the recursor format described in
  // https://lean-lang.org/doc/reference/latest/The-Type-System/Inductive-Types/#recursor-types.
  // This treats the equality type as an inductive with one parameter and one
  // index.

  static const std::string replace_type_str =
      ("Pi (parameter: Nat)."
       "Pi (motive: Pi (index: Nat). Pi (value: parameter = index). Type)."
       "Pi (minor_premise: motive parameter (refl parameter))."
       "Pi (major_premise_index: Nat)."
       "Pi (major_premise: (parameter = major_premise_index))."
       "motive major_premise_index major_premise");
  expression_type_map_[&e] = text_to_expression(replace_type_str);
}

void TypeGenVisitor::visit_reflexive_keyword(
    const ReflexiveKeywordExpression &e) {
  static const std::string refl_type_str =
      ("Pi (a: Nat)."
       "(a = a)");
  expression_type_map_[&e] = text_to_expression(refl_type_str);
}

void TypeGenVisitor::visit_succ_keyword(const SuccKeywordExpression &e) {
  static const std::string succ_type_str =
      ("Pi (n: Nat)."
       "Nat");
  expression_type_map_[&e] = text_to_expression(succ_type_str);
}

void TypeGenVisitor::visit_equality(const EqualityExpression &e) {
  static const std::string equality_type_str = ("Type");
  expression_type_map_[&e] = text_to_expression(equality_type_str);
}

}  // namespace script3025