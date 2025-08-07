#ifndef SCRIPT3025_SCRIPT3025_CORE_PARSING_UTILITY_HPP
#define SCRIPT3025_SCRIPT3025_CORE_PARSING_UTILITY_HPP

#include <unordered_set>

#include "parser.hpp"
#include "token.hpp"

namespace script3025 {

struct AnnotatedToken {
  Token token;
  std::string text;
};

struct ParsedCode {
  std::unique_ptr<parser::ConcreteSyntaxTree<Token>> cst;
  std::vector<AnnotatedToken> annotated_tokens;
};

[[nodiscard]] std::vector<AnnotatedToken> tokenize(const std::string& text);
[[nodiscard]] ParsedCode parse(const std::string& text);

}  // namespace script3025

#endif
