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
  LOGGER3025_INFO("collect_lists produced the following CST:\n"
                  "{}", to_string(*code.cst));

  script3025::collapse_oop(*code.cst);
  LOGGER3025_INFO("collapse_oop produced the following CST:\n"
                  "{}", to_string(*code.cst));
}

TEST(Evaluate, simple) {
  script3025::ParsedCode code = script3025::parse(
    "let identity := lambda (x : Type). x"
  );
  script3025::collect_lists(*code.cst);
  script3025::collapse_oop(*code.cst);

  LOGGER3025_INFO("CST:\n"
                  "{}", to_string(*code.cst));

  std::vector<std::string> token_text;
  for (size_t i = 0; i < code.segment_begin.size(); ++i) {
    token_text.push_back(
        code.text.substr(code.segment_begin[i],
        code.segment_end[i] - code.segment_begin[i]));
  }

  auto iter = token_text.begin();
  std::unique_ptr<script3025::Expression> expression =
      script3025::Expression::create(code.cst -> children[0], iter);
  LOGGER3025_INFO("{}", to_string(*expression));
}