#ifndef SCRIPT3025_SCRIPT3025_CORE_EXPRESSION_HPP
#define SCRIPT3025_SCRIPT3025_CORE_EXPRESSION_HPP

#include <string>

#include "token.hpp"
#include "parser.hpp"

namespace script3025 {

class ExpressionVisitor;
class MutatingExpressionVisitor;

class Expression {
 public:
  // @brief
  // Creates an `Expression` based on a CST and a token iterator.
  // `source` is the CST that this expression is based on, and `string_iterator`
  // is the list of strings associated with each leaf node in `source`.
  // This uses move semantics, so this might damage the underlying container for
  // the iterator.
  template <typename Iterator>
  static std::unique_ptr<Expression> create(
      const parser::ConcreteSyntaxTree<Token> &source,
      Iterator &string_iterator);

  // @brief
  // Returns true if this expression is in normal form within its context
  virtual bool is_normal() const = 0;

  virtual ~Expression() = default;

  std::unique_ptr<Expression> clone(
      std::unordered_map<const Expression *, Expression *> initial_map = {});

  // @brief
  // Returns true iff the 2 expressions are syntactically equal.
  // 2 expressions might reduce to the same normal form but not be equal in this
  // sense.
  bool operator==(const Expression& other) const;

  // @brief
  // Returns the negation of `operator==`.
  bool operator!=(const Expression& other) const;

  virtual std::ostream &print(std::ostream &os) const = 0;

  // @brief
  // Converts the current expression into a string
  std::string to_string();

  virtual void accept(ExpressionVisitor &visitor) const = 0;

  virtual void accept(MutatingExpressionVisitor &visitor) = 0;

  static std::shared_ptr<spdlog::logger> get_logger();
};

inline std::ostream &operator<<(std::ostream &os, const Expression &expr) {
  return expr.print(os);
}

} // namespace script3025

#endif