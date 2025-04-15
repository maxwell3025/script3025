#include <gtest/gtest.h>

#include "script3025_core.hpp"
#include "parsing_utility.hpp"

TEST(Parse, single_defn_simple) {
  script3025::parse(
    "let identity := lambda (x : Type). x"
  );
}

TEST(Parse, single_defn_complex) {
  script3025::parse(
    "let identity := lambda (x : Type). lambda (y : Type). lambda (z : Type). x"
  );
}

TEST(Parse, multi_defn) {
  script3025::ParsedCode code = script3025::parse(
    "let identity := lambda (x : Type). x\n"
    "let identity_two := lambda (y : Type). y"
  );

  script3025::collect_lists(*code.cst);
  script3025::collapse_oop(*code.cst);

  LOGGER3025_INFO("Recieved the following CST:\n"
                  "{}", to_string(*code.cst));
}