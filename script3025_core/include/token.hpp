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
  PERIOD,
  COLON,
  LET,
  INDUCTIVE,
  DEF,
  L_PAREN,
  R_PAREN,
  // non-terminals
  // This is an actual abstraction, as opposed to an abstraction-level expression.
  ABS,
  EXPR_PAREN,
  EXPR_APP,
  // This is the lowest-precedence expression. Use this whn you just need a generic expressionn.
  EXPR_ABS,
  EXPR = EXPR_ABS,
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
  template<typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template<typename FormatContext>
  auto format(script3025::Token const& token, FormatContext& ctx) const {
    std::stringstream output;
    script3025::operator<<(output, token);
    // output << token;
    return fmt::format_to(ctx.out(), "{0}", output.str());
  }
};

#endif