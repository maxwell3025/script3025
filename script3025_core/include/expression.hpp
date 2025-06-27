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
  // Creates a copy of the this expression.
  virtual std::unique_ptr<Expression> clone() const = 0;

  // @brief
  // Returns true if this expression is in normal form within its context
  virtual bool is_normal() const = 0;

  // @brief
  // Gets the type of this expression in normal form.
  // This also determines if the expression is well-typed.
  // For non-well-typed expressions, this expression is false.
  virtual Expression* get_type() = 0;

  // @brief
  // Replaces all identifiers matching `id` which came from the abstraction
  // `source` with `expression`.
  virtual std::unique_ptr<Expression> replace(const std::string &id,
                                              const Expression *source,
                                              const Expression &expression) = 0;

  // @brief
  // Reduces this expression to normal form within its context.
  virtual std::unique_ptr<Expression> reduce() = 0;

  virtual ~Expression() = default;

  // @brief
  // Returns true iff the 2 expressions are syntactically equal.
  // 2 expressions might reduce to the same normal form but not be equal in this
  // sense.
  virtual bool operator==(const Expression& other) const = 0;

  // @brief
  // Returns the negation of `operator==`.
  bool operator!=(const Expression& other) const;

  virtual std::ostream &print(std::ostream &os) const = 0;

  // @brief
  // The deepest abstraction (Pi or lambda) that this expression is inside of.
  // For global expressions, `parent_abstraction` should be `nullptr`.
  Expression *parent_abstraction;

  // @brief
  // A pointer to the type of this expression.
  // If the type was never evaluated, then this might be null.
  std::unique_ptr<Expression> type;

  // @brief
  // This is true if this expression might be well-typed.
  // This is intially true, but if a type error is detected, then this becomes
  // false.
  bool possibly_well_typed;

  // @brief
  // Converts the current expression into a string
  std::string to_string();

  virtual void accept(ExpressionVisitor &visitor) const = 0;

  virtual void accept(MutatingExpressionVisitor &visitor) = 0;

 protected:
  Expression(Expression *parent_abstraction, std::unique_ptr<Expression> &&type,
             bool possibly_well_typed);

  Expression(const Expression& other);

  Expression();

  static std::shared_ptr<spdlog::logger> get_logger();
};

inline std::ostream &operator<<(std::ostream &os, const Expression &expr) {
  return expr.print(os);
}

} // namespace script3025

#endif