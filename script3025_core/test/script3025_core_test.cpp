#include "script3025_core.hpp"

#include <gtest/gtest.h>

#include <memory>

#include "expression/expression.hpp"
#include "spdlog/common.h"
#include "spdlog/logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

[[nodiscard]] std::shared_ptr<spdlog::logger> get_logger() {
  static std::shared_ptr<spdlog::logger> logger =
      ([&]() -> std::shared_ptr<spdlog::logger> {
        logger = spdlog::stderr_color_mt("script3025_core_test.cpp",
                                         spdlog::color_mode::always);
        logger->set_level(spdlog::level::info);
        logger->set_pattern("%^[%l] [tid=%t] [%T.%F] [%s:%#] %v%$");
        return logger;
      })();
  return logger;
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

TEST(Program, interpreter_hard) {
  script3025::Program program(
      "def add := lambda (a : Nat). lambda (b: Nat). inductive (lambda (k: Nat).Nat) succ a b\n"
      "def mul := lambda (a : Nat). lambda (b: Nat). inductive (lambda (k: Nat).Nat) (lambda (s: Nat). add s a) 0 b\n"
      "def bar := mul 12 4");

  SPDLOG_LOGGER_INFO(get_logger(), "\n{}", program);

  std::unique_ptr<script3025::Expression> result = program.reduce("bar");
  SPDLOG_LOGGER_INFO(get_logger(), "Reduced form of bar:\n{}", *result);
}

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
