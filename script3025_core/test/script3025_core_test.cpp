#include <gtest/gtest.h>

#include "script3025_core.hpp"
#include "parsing_utility.hpp"

TEST(Parse, basic) {
  script3025::parse(
    "let type_identity := (a b c d lambda (x : Type). x y z w)"
  );
}
TEST(Parse, un_basic) {
  script3025::ParsedCode code = script3025::parse(
    "let type_identity := (a b c d lambda (x : Type). x y z w)"
    "let other_func := (a b c d lambda (x : Type). x y z w)"
  );
  script3025::collect_lists(*code.cst);
  script3025::collapse_oop(*code.cst);

  LOGGER3025_INFO("Recieved the following CST:\n"
                  "{}", to_string(*code.cst));
}