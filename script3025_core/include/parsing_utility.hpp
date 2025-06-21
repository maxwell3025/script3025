#ifndef SCRIPT3025_SCRIPT3025_CORE_PARSING_UTILITY_HPP
#define SCRIPT3025_SCRIPT3025_CORE_PARSING_UTILITY_HPP

#include <unordered_set>

#include "token.hpp"
#include "parser.hpp"


namespace script3025 {

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
