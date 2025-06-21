#include <gtest/gtest.h>

#include "script3025_core.hpp"
#include "parsing_utility.hpp"

std::shared_ptr<spdlog::logger> get_logger() {
  static std::shared_ptr<spdlog::logger> logger =
      ([&] () -> std::shared_ptr<spdlog::logger> {
        logger = spdlog::stderr_color_mt("script3025_core_test.cpp", spdlog::color_mode::always);
        logger-> set_level(spdlog::level::warn);
        logger-> set_pattern("%^[%l] [tid=%t] [%T.%F] [%s:%#] %v%$");
        return logger;
      })();
  return logger;
}

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
  SPDLOG_LOGGER_INFO(get_logger(), "collect_lists produced the following CST:\n"
                  "{}", code.cst -> to_string());

  script3025::collapse_oop(*code.cst);
  SPDLOG_LOGGER_INFO(get_logger(), "collapse_oop produced the following CST:\n"
                  "{}", code.cst -> to_string());
}

TEST(Evaluate, simple) {
  script3025::ParsedCode code = script3025::parse(
    "let identity := lambda (x : Type). x"
  );
  script3025::collect_lists(*code.cst);
  script3025::collapse_oop(*code.cst);

  SPDLOG_LOGGER_INFO(get_logger(), "CST:\n"
                  "{}", code.cst -> to_string());

  std::vector<std::string> token_text;
  for (size_t i = 0; i < code.segment_begin.size(); ++i) {
    token_text.push_back(
        code.text.substr(code.segment_begin[i],
        code.segment_end[i] - code.segment_begin[i]));
  }

  auto iter = token_text.begin();
  std::unique_ptr<script3025::Expression> expression =
      script3025::Expression::create(code.cst -> children[0], iter);
  SPDLOG_LOGGER_INFO(get_logger(), "{}", expression -> to_string());
}