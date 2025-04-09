#include <gtest/gtest.h>

#include "script3025_core.hpp"
#include "parsing_utility.hpp"

TEST(Parse, basic) {
  script3025::parse(
    "let type_identity := (a b c d lambda (x : Type). x y z w)"
  );
}