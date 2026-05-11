#include "script3025_core.hpp"

#include <gtest/gtest.h>

#include <memory>

#include "expression/expression.hpp"
#include "expression/visitors/lazy_reduction_visitor.hpp"
#include "expression/visitors/type_gen_visitor.hpp"
#include "expression_factory.hpp"
#include "spdlog/common.h"
#include "spdlog/logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

[[nodiscard]] std::shared_ptr<spdlog::logger> get_logger() {
  static std::shared_ptr<spdlog::logger> logger =
      ([&]() -> std::shared_ptr<spdlog::logger> {
        logger = spdlog::stderr_color_mt("script3025_core_test.cpp",
                                         spdlog::color_mode::always);
        logger->set_level(spdlog::level::trace);
        logger->set_pattern("%^[%l] [tid=%t] [%T.%F] [%s:%#] %v%$");
        return logger;
      })();
  return logger;
}

TEST(text_to_expression, identifier) {
  auto expression = script3025::text_to_expression("anonymous_identifier");
  SPDLOG_LOGGER_INFO(get_logger(), "\n{}", expression->to_string());
}

TEST(text_to_expression, built_in) {
  auto expression = script3025::text_to_expression("Nat");
  SPDLOG_LOGGER_INFO(get_logger(), "\n{}", expression->to_string());
}

TEST(text_to_expression, lambda) {
  auto expression = script3025::text_to_expression("lambda (x: Nat). x");
  SPDLOG_LOGGER_INFO(get_logger(), "\n{}", expression->to_string());
}

TEST(text_to_expression, Pi) {
  auto expression = script3025::text_to_expression("Pi (x: Nat). x");
  SPDLOG_LOGGER_INFO(get_logger(), "\n{}", expression->to_string());
}

TEST(type_gen_visitor, simple) {
  auto expression = script3025::text_to_expression("Nat");
  auto expected_type = script3025::text_to_expression("Type");
  script3025::TypeGenVisitor type_gen_visitor{{}, {}};
  type_gen_visitor.visit(*expression);
  EXPECT_EQ(*type_gen_visitor.get_type(expression.get()), *expected_type);
}

TEST(type_gen_visitor, application) {
  auto expression = script3025::text_to_expression(
      "("
      "lambda (b: Nat)."
      "lambda (a: Pi (x: Nat). Type)."
      "a b"
      ") 0");
  auto expected_type =
      script3025::text_to_expression("Pi (a: Pi (x: Nat). Type). Type");
  script3025::TypeGenVisitor type_gen_visitor{{}, {}};
  type_gen_visitor.visit(*expression);
  if (type_gen_visitor.get_type(expression.get()) == nullptr) {
    SPDLOG_LOGGER_ERROR(get_logger(),
                        "Type generation failed for expression:\n{}",
                        expression->to_string());
    FAIL() << "Type generation failed for expression:\n"
           << expression->to_string();
  }
  EXPECT_EQ(*type_gen_visitor.get_type(expression.get()), *expected_type);
}

TEST(type_gen_visitor, Pi_basic) {
  auto expression = script3025::text_to_expression("Pi (x: Nat). Type");
  auto expected_type = script3025::text_to_expression("Type 1");
  script3025::TypeGenVisitor type_gen_visitor{{}, {}};
  type_gen_visitor.visit(*expression);
  if (type_gen_visitor.get_type(expression.get()) == nullptr) {
    SPDLOG_LOGGER_ERROR(get_logger(),
                        "Type generation failed for expression:\n{}",
                        expression->to_string());
    FAIL() << "Type generation failed for expression:\n"
           << expression->to_string();
  }
  EXPECT_EQ(*type_gen_visitor.get_type(expression.get()), *expected_type);
}

TEST(type_gen_visitor, let_expression_easy) {
  auto expression = script3025::text_to_expression("let (x: Nat) := 0 in x");
  auto expected_type = script3025::text_to_expression("Nat");
  script3025::TypeGenVisitor type_gen_visitor{{}, {}};
  type_gen_visitor.visit(*expression);
  if (type_gen_visitor.get_type(expression.get()) == nullptr) {
    SPDLOG_LOGGER_ERROR(get_logger(),
                        "Type generation failed for expression:\n{}",
                        expression->to_string());
    FAIL() << "Type generation failed for expression:\n"
           << expression->to_string();
  }
  EXPECT_EQ(*type_gen_visitor.get_type(expression.get()), *expected_type);
}

TEST(type_gen_visitor, induction) {
  auto expression = script3025::text_to_expression("inductive");
  auto expected_type = script3025::text_to_expression(
      "Pi (motive: Pi (input: Nat). Type)."
      "Pi (minor_premise_1: Pi (n: Nat). Pi (prev: motive n). motive (succ "
      "n))."
      "Pi (minor_premise_2: motive 0)."
      "Pi (major_premise: Nat)."
      "motive major_premise");
  script3025::TypeGenVisitor type_gen_visitor{{}, {}};
  type_gen_visitor.visit(*expression);
  if (type_gen_visitor.get_type(expression.get()) == nullptr) {
    SPDLOG_LOGGER_ERROR(get_logger(),
                        "Type generation failed for expression:\n{}",
                        expression->to_string());
    FAIL() << "Type generation failed for expression:\n"
           << expression->to_string();
  }
  EXPECT_EQ(*type_gen_visitor.get_type(expression.get()), *expected_type);
}

TEST(type_gen_visitor, complex_application) {
  auto expression = script3025::text_to_expression(
      "inductive (lambda (k: Nat). Nat) (lambda (k: Nat). succ) 1 1");
  auto expected_type = script3025::text_to_expression("Nat");
  script3025::TypeGenVisitor type_gen_visitor{{}, {}};
  type_gen_visitor.visit(*expression);
  if (type_gen_visitor.get_type(expression.get()) == nullptr) {
    SPDLOG_LOGGER_ERROR(get_logger(),
                        "Type generation failed for expression:\n{}",
                        expression->to_string());
    FAIL() << "Type generation failed for expression:\n"
           << expression->to_string();
  }
  EXPECT_EQ(*type_gen_visitor.get_type(expression.get()), *expected_type);
}

TEST(type_gen_visitor, addition) {
  auto expression = script3025::text_to_expression(
      "lambda (a: Nat).\n"
      "lambda (b: Nat).\n"
      "inductive (lambda (k: Nat). Nat) (lambda (k: Nat). succ) a b");
  auto expected_type =
      script3025::text_to_expression("Pi (a: Nat). Pi (b: Nat). Nat");
  script3025::TypeGenVisitor type_gen_visitor{{}, {}};
  type_gen_visitor.visit(*expression);
  if (type_gen_visitor.get_type(expression.get()) == nullptr) {
    SPDLOG_LOGGER_ERROR(get_logger(),
                        "Type generation failed for expression:\n{}",
                        expression->to_string());
    FAIL() << "Type generation failed for expression:\n"
           << expression->to_string();
  }
  EXPECT_EQ(*type_gen_visitor.get_type(expression.get()), *expected_type);
}

TEST(type_gen_visitor, eq_comm_proof) {
  auto expression = script3025::text_to_expression(
      // clang-format off
      "lambda (a: Nat).\n"
      "lambda (b: Nat).\n"
      "lambda (eq: (b = a)).\n"
      "  replace\n"
      "    b\n"
      "    (lambda (index: Nat). lambda (value: b = index). (index = b))\n"
      "    (refl b)\n"
      "    a\n"
      "    eq");
  // clang-format on
  auto expected_type = script3025::text_to_expression(
      "Pi (a: Nat). Pi (b: Nat). Pi (eq: (b = a)). (a = b)");
  script3025::TypeGenVisitor type_gen_visitor{{}, {}};
  type_gen_visitor.visit(*expression);
  if (type_gen_visitor.get_type(expression.get()) == nullptr) {
    SPDLOG_LOGGER_ERROR(get_logger(),
                        "Type generation failed for expression:\n{}",
                        expression->to_string());
    FAIL() << "Type generation failed for expression:\n"
           << expression->to_string();
  }
  EXPECT_EQ(*type_gen_visitor.get_type(expression.get()), *expected_type);
}

TEST(type_gen_visitor, eq_trans_proof) {
  auto expression = script3025::text_to_expression(
      // clang-format off
      "lambda (a: Nat).\n"
      "lambda (b: Nat).\n"
      "lambda (c: Nat).\n"
      "lambda (eq_2: (a = b)).\n"
      "lambda (eq_1: (b = c)).\n"
      "  replace\n"
      "    b\n"
      "    (lambda (index: Nat). lambda (value: b = index). (a = index))\n"
      "    eq_2\n"
      "    c\n"
      "    eq_1");
  // clang-format on
  auto expected_type = script3025::text_to_expression(
      "Pi (a: Nat). Pi (b: Nat). Pi (c: Nat). Pi (eq: (a = b)). Pi (eq: (b = "
      "c)). (a = c)");
  script3025::TypeGenVisitor type_gen_visitor{{}, {}};
  type_gen_visitor.visit(*expression);
  if (type_gen_visitor.get_type(expression.get()) == nullptr) {
    SPDLOG_LOGGER_ERROR(get_logger(),
                        "Type generation failed for expression:\n{}",
                        expression->to_string());
    FAIL() << "Type generation failed for expression:\n"
           << expression->to_string();
  }
  EXPECT_EQ(*type_gen_visitor.get_type(expression.get()), *expected_type);
}

TEST(type_gen_visitor, add_comm_proof) {
  auto expression = script3025::text_to_expression(
      // clang-format off
      "lambda (a: Nat).\n"
      "lambda (b: Nat).\n"
      "lambda (c: Nat).\n"
      "lambda (eq_2: (a = b)).\n"
      "lambda (eq_1: (b = c)).\n"
      "  replace\n"
      "    b\n"
      "    (lambda (index: Nat). lambda (value: b = index). (a = index))\n"
      "    eq_2\n"
      "    c\n"
      "    eq_1");
  // clang-format on
  auto expected_type = script3025::text_to_expression(
      "Pi (a: Nat). Pi (b: Nat). Pi (c: Nat). Pi (eq: (a = b)). Pi (eq: (b = "
      "c)). (a = c)");
  script3025::TypeGenVisitor type_gen_visitor{{}, {}};
  type_gen_visitor.visit(*expression);
  if (type_gen_visitor.get_type(expression.get()) == nullptr) {
    SPDLOG_LOGGER_ERROR(get_logger(),
                        "Type generation failed for expression:\n{}",
                        expression->to_string());
    FAIL() << "Type generation failed for expression:\n"
           << expression->to_string();
  }
  EXPECT_EQ(*type_gen_visitor.get_type(expression.get()), *expected_type);
}

TEST(type_gen_visitor, delta_reduction) {
  // This test makes sure that the type generator is able to do delta reductions
  // when generating types.
  auto expression = script3025::text_to_expression(
      "lambda (a: Nat).\n"
      "  let (T: Type) := Nat in\n"
      "  let (b: T) := a in\n"
      "  b");
  auto expected_type = script3025::text_to_expression("Pi (a: Nat). Nat");
  script3025::TypeGenVisitor type_gen_visitor{{}, {}};
  type_gen_visitor.visit(*expression);
  if (type_gen_visitor.get_type(expression.get()) == nullptr) {
    SPDLOG_LOGGER_ERROR(get_logger(),
                        "Type generation failed for expression:\n{}",
                        expression->to_string());
    FAIL() << "Type generation failed for expression:\n"
           << expression->to_string();
  }
  EXPECT_EQ(*type_gen_visitor.get_type(expression.get()), *expected_type);
}

TEST(lazy_reduction_visitor, simple) {
  auto expression = script3025::text_to_expression("(lambda (x: Nat). x) 100");
  auto expected_reduction = script3025::text_to_expression("100");

  auto actual_reduction = reduce_copy(*expression, {});
  if (actual_reduction == nullptr) {
    SPDLOG_LOGGER_ERROR(get_logger(), "Reduction failed for expression:\n{}",
                        expression->to_string());
    FAIL() << "Reduction failed for expression:\n" << expression->to_string();
  }
  EXPECT_EQ(*actual_reduction, *expected_reduction);
}

TEST(lazy_reduction_visitor, addition) {
  auto expression = script3025::text_to_expression(
      ("let add: Pi(a: Nat). Pi(b: Nat). Nat :=\n"
      //  "  lambda (a: Nat).\n"
      //  "  lambda (b: Nat).\n"
       "  inductive (lambda (k: Nat). Nat)\n"
       "            (lambda (_: Nat). succ)\n"
      //  "            a\n"
      //  "            b\n"
       "in\n"
       "\n"
       "add 12 4\n"));
  auto expected_reduction = script3025::text_to_expression("16");

  auto actual_reduction = reduce_copy(*expression, {});
  if (actual_reduction == nullptr) {
    SPDLOG_LOGGER_ERROR(get_logger(), "Reduction failed for expression:\n{}",
                        expression->to_string());
    FAIL() << "Reduction failed for expression:\n" << expression->to_string();
  }
  EXPECT_EQ(*actual_reduction, *expected_reduction);
}

TEST(Program, simple) {
  script3025::Program program("def identity := lambda (x : Type). x");
  SPDLOG_LOGGER_INFO(get_logger(), "\n{}", program);
  EXPECT_TRUE(program.check_types()) << "program failed to type check";
}

TEST(Program, multi) {
  const script3025::Program program(
      "def foo := lambda (x : Type). x \n"
      "def bar := lambda (x : Type). foo x");

  SPDLOG_LOGGER_INFO(get_logger(), "\n{}", program);
}

TEST(Program, interpreter) {
  script3025::Program program(
      "def foo := lambda (x : Nat). x \n"
      "def bar := foo 1");

  SPDLOG_LOGGER_INFO(get_logger(), "\n{}", program);

  std::unique_ptr<script3025::Expression> result = program.reduce("bar");
  SPDLOG_LOGGER_INFO(get_logger(), "Reduced form of bar:\n{}", *result);
}

TEST(Program, interpreter_hard_incorrect) {
  // The program is incorrectly typed because the minor premise of add (succ)
  // needs to be parametric over the predecessor. This should still not segfault
  // though.
  std::string const program_source =
      ("def add :=\n"
       "  lambda (a: Nat).\n"
       "  lambda (b: Nat).\n"
       "  inductive (lambda (k: Nat). Nat)\n"
       "            succ\n"
       "            a\n"
       "            b\n"
       "\n"
       "def mul :=\n"
       "  lambda (a: Nat).\n"
       "  lambda (b: Nat).\n"
       "  inductive (lambda (k: Nat). Nat)\n"
       "            (lambda (s: Nat). add s a)\n"
       "            0\n"
       "            b\n"
       "\n"
       "def bar :=\n"
       "  mul 12 4\n");
  script3025::Program program(program_source);
  SPDLOG_LOGGER_INFO(get_logger(), "Built Program:\n{}", program);
  auto result = program.reduce("bar");
  SPDLOG_LOGGER_INFO(get_logger(), "Computed bar:\n{}", *result);
  auto expected_result = script3025::text_to_expression("48");

  // The reduction process should spin out of control and produce some
  // O(48)-sized expression.
  EXPECT_NE(*result, *expected_result);
}

TEST(Program, interpreter_hard_correct) {
  std::string const program_source =
      ("def add :=\n"
       "  lambda (a: Nat).\n"
       "  lambda (b: Nat).\n"
       "  inductive (lambda (k: Nat). Nat)\n"
       "            (lambda (_: Nat). succ)\n"
       "            a\n"
       "            b\n"
       "\n"
       "def mul :=\n"
       "  lambda (a: Nat).\n"
       "  lambda (b: Nat).\n"
       "  inductive (lambda (k: Nat). Nat)\n"
       "            (lambda (_: Nat). add a)\n"
       "            0\n"
       "            b\n"
       "\n"
       "def bar :=\n"
       "  mul 12 4\n");
  script3025::Program program(program_source);
  SPDLOG_LOGGER_INFO(get_logger(), "Built Program:\n{}", program);
  auto result = program.reduce("bar");
  SPDLOG_LOGGER_INFO(get_logger(), "Computed bar:\n{}", *result);
  auto expected_result = script3025::text_to_expression("48");

  EXPECT_EQ(*result, *expected_result);
}

TEST(Program, interpreter_hard_typed_incorrect) {
  // The program is incorrectly typed because the minor premise of add (succ)
  // needs to be parametric over the predecessor. This should still not segfault
  // though.
  std::string const program_source =
      ("def add :=\n"
       "  lambda (a: Nat).\n"
       "  lambda (b: Nat).\n"
       "  inductive (lambda (k: Nat). Nat)\n"
       "            succ\n"
       "            a\n"
       "            b\n"
       "\n"
       "def mul :=\n"
       "  lambda (a: Nat).\n"
       "  lambda (b: Nat).\n"
       "  inductive (lambda (k: Nat). Nat)\n"
       "            (lambda (s: Nat). add s a)\n"
       "            0\n"
       "            b\n"
       "\n"
       "def bar :=\n"
       "  mul 12 4\n");

  script3025::Program program(program_source);

  SPDLOG_LOGGER_INFO(get_logger(), "\n{}", program);

  program.check_types();
  std::unique_ptr<script3025::Expression> result = program.reduce("bar");
  SPDLOG_LOGGER_INFO(get_logger(), "Reduced form of bar:\n{}", *result);
}

TEST(Program, interpreter_hard_typed_correct) {
  std::string const program_source =
      ("def add :=\n"
       "  lambda (a: Nat).\n"
       "  lambda (b: Nat).\n"
       "  inductive (lambda (k: Nat). Nat)\n"
       "            (lambda (_: Nat). succ)\n"
       "            a\n"
       "            b\n"
       "\n"
       "def mul :=\n"
       "  lambda (a: Nat).\n"
       "  lambda (b: Nat).\n"
       "  inductive (lambda (k: Nat). Nat)\n"
       "            (lambda (_: Nat). add a)\n"
       "            0\n"
       "            b\n"
       "\n"
       "def bar :=\n"
       "  mul 12 4\n");
  script3025::Program program(program_source);

  SPDLOG_LOGGER_INFO(get_logger(), "\n{}", program);

  program.check_types();
  std::unique_ptr<script3025::Expression> result = program.reduce("bar");
  SPDLOG_LOGGER_INFO(get_logger(), "Reduced form of bar:\n{}", *result);
}

TEST(Program, equality_properties) {
  std::string const program_source =
      // clang-format off
      ("def eq_comm :=\n"
       "  lambda (a: Nat).\n"
       "  lambda (b: Nat).\n"
       "  lambda (eq: (a = b)).\n"
       "    let (parameter: Nat) := a in\n"
       "    let (motive: Pi (index: Nat). Pi (value: parameter = index). Type) :=\n"
       "      lambda (index: Nat).\n"
       "      lambda (value: parameter = index).\n"
       "        (a = index) in\n"
       "    let (minor_premise: motive parameter (refl parameter)) := refl a "
       "in\n"
       "    let (major_premise_index: Nat) := b in\n"
       "    let (major_premise: (parameter = major_premise_index)) := eq in\n"
       "    replace parameter motive minor_premise major_premise_index "
       "major_premise");
  // clang-format on
  script3025::Program program(program_source);

  SPDLOG_LOGGER_INFO(get_logger(), "\n{}", program);

  program.check_types();
  std::unique_ptr<script3025::Expression> result = program.reduce("eq_comm");
  SPDLOG_LOGGER_INFO(get_logger(), "Reduced form of eq_comm:\n{}", *result);
}

TEST(Program, equality_properties_no_let) {
  std::string const program_source =
      ("def eq_comm :=\n"
       "  lambda (a: Nat).\n"
       "  lambda (b: Nat).\n"
       "  lambda (eq: (b = a)).\n"
       "    replace\n"
       "      b\n"
       "      (lambda (index: Nat). lambda (value: b = index). (index = b))\n"
       "      (refl b)\n"
       "      a\n"
       "      eq");
  script3025::Program program(program_source);

  SPDLOG_LOGGER_INFO(get_logger(), "\n{}", program);

  program.check_types();
  std::unique_ptr<script3025::Expression> result = program.reduce("eq_comm");
  SPDLOG_LOGGER_INFO(get_logger(), "Reduced form of eq_comm:\n{}", *result);
}

// TEST(Program, addition_commutativity) {
//   std::string const program_source =
//       ("def add :=\n"
//        "  lambda (a: Nat).\n"
//        "  lambda (b: Nat).\n"
//        "  inductive (lambda (k: Nat). Nat)\n"
//        "            (lambda (_: Nat). succ)\n"
//        "            a\n"
//        "            b\n"
//        "\n"
//        "def zero_comm :=\n"
//        "  lambda (a: Nat).\n"
//        "    let (motive: Pi (k: Nat). Type) := lambda (k: Nat). (add 0 k =
//        add k 0) in\n" "    let (base_case: motive 0 := refl (add 0 0) in\n"
//        "    let (minor_premise: Pi (n: Nat). Pi (prev: motive n). motive
//        (succ n)) :=\n" "      refl (add 0 (succ n))\n" "    in\n" " reflexive
//        (add 0 0)\n"
//     );
//   script3025::Program program(program_source);

//   SPDLOG_LOGGER_INFO(get_logger(), "\n{}", program);

//   program.check_types();
//   std::unique_ptr<script3025::Expression> result = program.reduce("bar");
//   SPDLOG_LOGGER_INFO(get_logger(), "Reduced form of bar:\n{}", *result);
// }

/*
  // induction is forall prop: prop(0) and prop(n) => prop(n + 1) implies
  prop(n)
  //
  // ind: Pi (prop:
            Pi (k: Nat).
            Type).
          Pi (instance_0:
            prop 0).
          Pi (goal:
            Pi (hypothesis: prop n).
            prop (succ n)).
          Pi (n:
            Nat).
          prop(n)
  // ind is implemented as primitive recursion, so if you don't need type
  variance, prop can be a
  // hard-coded lambda

  + :=  lambda x : Nat.
        lambda y : Nat.
          ind (lambda _: Nat. Nat) (x) (lambda sum. succ sum) (y)

  // General Proof
  //  a + b = b + a
  //    induction over b:
  //    a + 0 = 0 + a
  //      induction over a:
  //      0 + 0 = 0 + 0
  //      succ a + 0 = 0 + succ a
  //        succ a + 0 = succ (0 + a)
  //          succ a + 0 = succ (0 + a)
  //            succ a = succ (0 + a)
  //              succ a = succ a
  //    a + succ b = succ b + a
  //      induction over a:
  //      0 + succ b = succ b + 0
  //        0 + succ b = succ b
  //
  //      succ a + succ b = succ b + succ a
  //        succ (succ a + b) = succ (succ b + a)
  //  The question is: how would I write eq.cast in my type system?
  //  So this means that Eq.rec
  //


  add_sym :=  lambda x : Nat.
              lambda y : Nat.
                rfl
*/
