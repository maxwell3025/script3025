#ifndef SCRIPT3025_SCRIPT3025_CORE_TOKEN_HPP
#define SCRIPT3025_SCRIPT3025_CORE_TOKEN_HPP

#include <iostream>

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

std::ostream &operator<<(std::ostream &os, script3025::Token token);

} // namespace script30225

#endif