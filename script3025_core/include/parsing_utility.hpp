#ifndef SCRIPT3025_SCRIPT3025_CORE_PARSING_UTILITY_HPP
#define SCRIPT3025_SCRIPT3025_CORE_PARSING_UTILITY_HPP

#include <memory>
#include <string>
#include <vector>

#include "parser.hpp"
#include "token.hpp"

namespace script3025 {

// @brief A `Token` with the segment of text that it corresponds to.
// @attribute `token` - The `Token` enum that the segment of text was parsed as.
// @attribute `text` - The segment of text that was parsed as a separate token.
struct AnnotatedToken {
  Token token;
  std::string text;
};

// @brief Concrete syntax tree for script3025 programs with leaf node <-> substring mappings.
// @attribute `cst` - The concrete syntx tree of the program.
// @attribute `annotated_tokens` - A vector of `AnnotatedToken`s representing the leaf node <-> substring mapping.
struct ParsedCode {
  std::unique_ptr<parser::ConcreteSyntaxTree<Token>> cst;
  std::vector<AnnotatedToken> annotated_tokens;
};

// @brief Convert a string of text into a sequence of `Token`s.
// @param `text` - The text that is going to be tokenized.
// @returns A vector of annotated tokens with the parsed tokens and the corresponding text.
// Note that this method erases whitespace.
[[nodiscard]] std::vector<AnnotatedToken> text_to_tokens(const std::string& text);

// @brief Convert a string of program source into a `ParsedCode` object.
[[nodiscard]] ParsedCode text_to_program_cst(const std::string& text);

// @brief Convert a string of expression source into a `ParsedCode` object.
[[nodiscard]] ParsedCode text_to_expression_cst(const std::string& text);

}  // namespace script3025

#endif
