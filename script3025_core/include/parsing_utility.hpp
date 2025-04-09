#ifndef PARSING_UTILITY_HPP
#define PARSING_UTILITY_HPP

#include "parser.hpp"
#include <unordered_set>

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
  DEFN,
  IDEFN,
  PROG,
  // terminal character
  END
};

std::ostream &operator<<(std::ostream &os, script3025::Token token);

class ParsedCode {
 public:
  std::string text;
  std::unique_ptr<parser::ConcreteSyntaxTree<Token>> cst;
  std::vector<size_t> segment_begin;
  std::vector<size_t> segment_end;
};

ParsedCode parse(std::string text);

} // namespace script3025

#endif
