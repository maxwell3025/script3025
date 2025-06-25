#ifndef SCRIPT3025_SCRIPT3025_CORE_PARSING_UTILITY_HPP
#define SCRIPT3025_SCRIPT3025_CORE_PARSING_UTILITY_HPP

#include <unordered_set>

#include "token.hpp"
#include "parser.hpp"


namespace script3025 {

struct AnnotatedToken {
  Token token;
  std::string text;
};

struct ParsedCode {
  std::unique_ptr<parser::ConcreteSyntaxTree<Token>> cst;
  std::vector<AnnotatedToken> annotated_tokens;
};

std::vector<AnnotatedToken> tokenize(std::string text);
ParsedCode parse(std::string text);

} // namespace script3025

#endif
