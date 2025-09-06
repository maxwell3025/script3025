#ifndef SCRIPT3025_SCRIPT3025_CORE_CST_TRANSFORMERS_HPP
#define SCRIPT3025_SCRIPT3025_CORE_CST_TRANSFORMERS_HPP

#include "parser.hpp"
#include "token.hpp"

namespace script3025 {

// @brief
// Collect all of the elements of list-like symbols.
//
// For instance, we want
// ```
// ┬prog
// ├┬prog
// │├┬prog
// ││├┬prog
// │││├─prog
// │││└─defn
// ││└─defn
// │└─defn
// └─defn
// ```
// To collapse into
// ```
// ┬prog
// ├─defn
// ├─defn
// ├─defn
// └─defn
// ```
void collect_lists(parser::ConcreteSyntaxTree<Token> &tree);

// @brief
// Collapse the order-of-operations rules in the CST.
void collapse_oop(parser::ConcreteSyntaxTree<Token> &tree);

}  // namespace script3025

#endif
