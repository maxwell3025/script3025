#include "script3025_core.hpp"

#include <gtest/gtest.h>

std::shared_ptr<spdlog::logger> get_logger() {
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

TEST(Parse, single_defn_simple) {
  script3025::ParsedCode parsed_code =
      script3025::parse("def identity := lambda (x : Type). x");
  script3025::collect_lists(*parsed_code.cst);
  script3025::collapse_oop(*parsed_code.cst);

  SPDLOG_LOGGER_INFO(get_logger(),
                     "CST:\n"
                     "{}",
                     parsed_code.cst->to_string());

  std::vector<std::string> token_text(parsed_code.annotated_tokens.size());
  std::transform(parsed_code.annotated_tokens.begin(),
                 parsed_code.annotated_tokens.end(), token_text.begin(),
                 [&](script3025::AnnotatedToken t) { return t.text; });

  auto string_iterator = token_text.begin();
  script3025::Program program(*(parsed_code.cst), string_iterator);

  for (const auto &definition : program.definitions) {
    SPDLOG_LOGGER_INFO(get_logger(), "{}:\n{}", definition.first,
                       *definition.second);
  }
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

TEST(Parse, single_defn_complex) {
  script3025::parse(
      "def identity := lambda (x : Type). lambda (y : Type). lambda (z : "
      "Type). x");
}

TEST(Parse, multi_defn) {
  script3025::ParsedCode code = script3025::parse(
      "def identity := lambda (x : Type). x\n"
      "def identity_two := lambda (y : Type). y");

  script3025::collect_lists(*code.cst);
  SPDLOG_LOGGER_INFO(get_logger(),
                     "collect_lists produced the following CST:\n"
                     "{}",
                     code.cst->to_string());

  script3025::collapse_oop(*code.cst);
  SPDLOG_LOGGER_INFO(get_logger(),
                     "collapse_oop produced the following CST:\n"
                     "{}",
                     code.cst->to_string());
}
