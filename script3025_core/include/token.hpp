#ifndef SCRIPT3025_SCRIPT3025_CORE_TOKEN_HPP
#define SCRIPT3025_SCRIPT3025_CORE_TOKEN_HPP

#include <iostream>
#include <sstream>

#include "spdlog/fmt/fmt.h"

namespace script3025 {

// @brief This represents the tokens in a parser
enum class Token {
  LAMBDA,
  PI,
  ID,
  NUMBER,
  EQ,
  PERIOD,
  COLON,
  LET,
  IN,
  DEF,
  INDUCTIVE,
  ASSIGN,
  L_PAREN,
  R_PAREN,
  // non-terminals
  // This is an actual abstraction, as opposed to an abstraction-level expression.
  ABS,
  EXPR_PAREN,
  EXPR_APP,
  // This is the lowest-precedence expression. Use this whn you just need a generic expressionn.
  EXPR_ABS,
  EXPR_EQ,
  EXPR = EXPR_EQ,
  DEFN,
  IDEFN,
  PROG,
  // terminal character
  END
};

std::ostream &operator<<(std::ostream &os, const script3025::Token &token);

} // namespace script30225

template <>
struct fmt::formatter<script3025::Token>
{
  constexpr auto parse(format_parse_context &ctx) {
    return ctx.begin();
  }

  auto format(script3025::Token const& token, format_context& ctx) const {
    std::stringstream output;
    output << token;
    return fmt::format_to(ctx.out(), "{0}", output.str());
  }
};

#endif