#ifndef SCRIPT3025_SCRIPT3025_CORE_EXPRESSION_HPP
#define SCRIPT3025_SCRIPT3025_CORE_EXPRESSION_HPP

#include <string>

#include "parser.hpp"
#include "token.hpp"

namespace script3025 {

class Expression {
 public:
  // @brief
  // Creates an `Expression` based on a CST and a token iterator.
  // `source` is the CST that this expression is based on, and `string_iterator`
  // is the list of strings associated with each leaf node in `source`.
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

 protected:
  Expression(Expression *parent_abstraction, std::unique_ptr<Expression> &&type,
             bool possibly_well_typed);

  Expression(const Expression& other);

  Expression();
};

class LambdaExpression : public Expression {
 public:
  LambdaExpression(std::string &&argument_id,
                   std::unique_ptr<Expression> &&argument_type,
                   std::unique_ptr<Expression> &&definition);
  LambdaExpression(const LambdaExpression &other);
  std::unique_ptr<Expression> clone() const override;
  bool is_normal() const override;
  Expression *get_type() override;
  std::unique_ptr<Expression> replace(const std::string &id,
                                      const Expression *source,
                                      const Expression &expression) override;
  std::unique_ptr<Expression> reduce() override;
  bool operator==(const Expression &other) const override;
  std::ostream &print(std::ostream &os) const override;

  std::string argument_id;
  std::unique_ptr<Expression> argument_type;
  std::unique_ptr<Expression> definition;
};

class PiExpression : public Expression {
 public:
  PiExpression(std::string &&argument_id,
               std::unique_ptr<Expression> &&argument_type,
               std::unique_ptr<Expression> &&definition);
  PiExpression(const PiExpression &other);
  std::unique_ptr<Expression> clone() const override;
  bool is_normal() const override;
  Expression *get_type() override;
  std::unique_ptr<Expression> replace(const std::string &id,
                                      const Expression *source,
                                      const Expression &expression) override;
  std::unique_ptr<Expression> reduce() override;
  bool operator==(const Expression &other) const override;
  std::ostream &print(std::ostream &os) const override;

  std::string argument_id;
  std::unique_ptr<Expression> argument_type;
  std::unique_ptr<Expression> definition;
 private:
};

class ApplicationExpression : public Expression {
 public:
  ApplicationExpression(std::unique_ptr<Expression> &&function,
                        std::unique_ptr<Expression> &&argument);
  ApplicationExpression(const ApplicationExpression &other);
  std::unique_ptr<Expression> clone() const override;
  bool is_normal() const override;
  Expression* get_type() override;
  std::unique_ptr<Expression> replace(const std::string &id,
                                      const Expression *source,
                                      const Expression &expression) override;
  std::unique_ptr<Expression> reduce() override;
  bool operator==(const Expression &other) const override;
  std::ostream &print(std::ostream &os) const override;

 private:
  /**
   * @brief converts an application expression into weak-head normal form by
   * recursively applying.
   * 
   * For example, if you have the expression F a b c d, this will first
   * apply a, then b, then c, then d.
   * 
   * this uses lazy evaluation, since a b c d are not reduced to normal form
   * first.
  */
  std::unique_ptr<Expression> get_whnf();

  std::unique_ptr<Expression> function;
  std::unique_ptr<Expression> argument;
};

class IdExpression : public Expression {
 public:
  IdExpression(std::string &&id, Expression *source);
  IdExpression(const IdExpression &other);
  std::unique_ptr<Expression> clone() const override;
  bool is_normal() const override;
  Expression *get_type() override;
  std::unique_ptr<Expression> replace(const std::string &id,
                                      const Expression *source,
                                      const Expression &expression) override;
  std::unique_ptr<Expression> reduce() override;
  bool operator==(const Expression &other) const override;
  std::ostream &print(std::ostream &os) const override;

 private:
  std::string id;
  Expression *source;
};

template <typename Iterator>
std::unique_ptr<Expression> Expression::create(
    const parser::ConcreteSyntaxTree<Token> &source,
    Iterator &string_iterator) {
  std::vector<Token> sentential_form;
  std::transform(source.children.begin(), source.children.end(),
      std::back_inserter(sentential_form),
      [](const parser::ConcreteSyntaxTree<Token>& child) -> Token {
        return child.symbol;
      });
  
  if (sentential_form == std::vector({Token::LAMBDA, Token::L_PAREN, Token::ID,
                                      Token::COLON, Token::EXPR, Token::R_PAREN,
                                      Token::PERIOD, Token::EXPR})) {
    // Lambda
    ++string_iterator;

    // (
    ++string_iterator;

    // Identifier
    std::string identifier = *string_iterator;
    ++string_iterator;

    // :
    ++string_iterator;

    // type bound
    std::unique_ptr<Expression> argument_type = Expression::create(
        source.children[4], string_iterator);

    // )
    ++string_iterator;

    // .
    ++string_iterator;

    // definition
    std::unique_ptr<Expression> definition = Expression::create(
        source.children[7], string_iterator);

    return std::make_unique<LambdaExpression>(identifier, argument_type,
        definition);
  } else if (sentential_form == std::vector({Token::LAMBDA, Token::ID,
                                             Token::COLON, Token::EXPR,
                                             Token::PERIOD, Token::EXPR})) {
    // Lambda
    ++string_iterator;

    // Identifier
    std::string identifier = *string_iterator;
    ++string_iterator;

    // :
    ++string_iterator;

    // type bound
    std::unique_ptr<Expression> argument_type = Expression::create(
        source.children[3], string_iterator);

    // .
    ++string_iterator;

    // definition
    std::unique_ptr<Expression> definition = Expression::create(
        source.children[5], string_iterator);

    return std::make_unique<LambdaExpression>(identifier, argument_type,
        definition);
  } else if (sentential_form == std::vector({Token::PI, Token::L_PAREN,
                                              Token::ID, Token::COLON,
                                              Token::EXPR, Token::R_PAREN,
                                              Token::PERIOD, Token::EXPR})) {
    // Lambda
    ++string_iterator;

    // (
    ++string_iterator;

    // Identifier
    std::string identifier = *string_iterator;
    ++string_iterator;

    // :
    ++string_iterator;

    // type bound
    std::unique_ptr<Expression> argument_type = Expression::create(
        source.children[4], string_iterator);

    // )
    ++string_iterator;

    // .
    ++string_iterator;

    // definition
    std::unique_ptr<Expression> definition = Expression::create(
        source.children[7], string_iterator);

    return std::make_unique<PiExpression>(identifier, argument_type,
        definition);
  } else if (sentential_form == std::vector({Token::LAMBDA, Token::ID,
                                             Token::COLON, Token::EXPR,
                                             Token::PERIOD, Token::EXPR})) {
    // Lambda
    ++string_iterator;

    // Identifier
    std::string identifier = *string_iterator;
    ++string_iterator;

    // :
    ++string_iterator;

    // type bound
    std::unique_ptr<Expression> argument_type = Expression::create(
        source.children[3], string_iterator);

    // .
    ++string_iterator;

    // definition
    std::unique_ptr<Expression> definition = Expression::create(
        source.children[5], string_iterator);

    return std::make_unique<PiExpression>(identifier, argument_type,
        definition);
  } else if (sentential_form == std::vector({Token::L_PAREN, Token::EXPR,
                                            Token::R_PAREN})) {
    // (
    ++string_iterator;

    // expression
    std::unique_ptr<Expression> inner_expression = Expression::create(
        source.children[1], string_iterator);

    // )
    ++string_iterator;

    return inner_expression;
  } else if (sentential_form == std::vector({Token::EXPR, Token::EXPR})) {
    std::unique_ptr<Expression> function = Expression::create(
        source.children[0], string_iterator);

    std::unique_ptr<Expression> argument = Expression::create(
        source.children[1], string_iterator);

    return std::make_unique<ApplicationExpression>(function, argument);
  } else {
    std::stringstream sentential_form_string;
    for (const Token &token : sentential_form) {
      sentential_form_string << token;
    }
    LOGGER3025_ERROR(
        "This sentential form failed to match any known expressions: {}",
        sentential_form_string.str());
    throw std::runtime_error("Failed to match token sequence.");
  }
}

std::ostream &operator<<(std::ostream &os, const Expression &expr) {
  return expr.print(os);
}

} // namespace script3025

#endif