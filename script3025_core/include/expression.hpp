#ifndef SCRIPT3025_SCRIPT3025_CORE_EXPRESSION_HPP
#define SCRIPT3025_SCRIPT3025_CORE_EXPRESSION_HPP

#include <string>

#include "parser.hpp"
#include "token.hpp"

namespace script3025 {

class Expression {
 public:
  template <typename Iterator>
  static std::unique_ptr<Expression> create(
      parser::ConcreteSyntaxTree<Token> &source, Iterator &string_iterator);
  std::unique_ptr<Expression> clone();
  virtual bool is_normal() const = 0;
  virtual std::unique_ptr<Expression> get_type() const = 0;
  virtual std::unique_ptr<Expression> replace(const std::string &id,
                                              const Expression &expression) 
                                              const = 0;
  virtual std::unique_ptr<Expression> get_reduced() const = 0;
  virtual ~Expression() = default;
};

class LambdaExpression : public Expression {
 public:
  LambdaExpression(std::string &&argument_id,
                   std::unique_ptr<Expression> &&argument_type,
                   std::unique_ptr<Expression> &&definition);
  LambdaExpression(const LambdaExpression &other);
  bool is_normal() const override;
  std::unique_ptr<Expression> get_type() const override;
  std::unique_ptr<Expression> replace(const std::string &id,
                                      const Expression &expression) const
                                      override;
  std::unique_ptr<Expression> get_reduced() const override;
 private:
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
  bool is_normal() const override;
  std::unique_ptr<Expression> get_type() const override;
  std::unique_ptr<Expression> replace(const std::string &id,
                                      const Expression &expression) const
                                      override;
  std::unique_ptr<Expression> get_reduced() const override;
 private:
  std::string argument_id;
  std::unique_ptr<Expression> argument_type;
  std::unique_ptr<Expression> definition;
};

class ApplicationExpression : public Expression {
 public:
  ApplicationExpression(std::unique_ptr<Expression> &&function,
                        std::unique_ptr<Expression> &&argument);
  ApplicationExpression(const ApplicationExpression &other);
  bool is_normal() const override;
  std::unique_ptr<Expression> get_type() const override;
  std::unique_ptr<Expression> replace(const std::string &id,
                                      const Expression &expression) const
                                      override;
  std::unique_ptr<Expression> get_reduced() const override;
 private:
  std::unique_ptr<Expression> function;
  std::unique_ptr<Expression> argument;
};

class IdExpression : public Expression {
 public:
  IdExpression(std::string &&id);
  IdExpression(const IdExpression &other);
  bool is_normal() const override;
  std::unique_ptr<Expression> get_type() const override;
  std::unique_ptr<Expression> replace(const std::string &id,
                                      const Expression &expression) const
                                      override;
  std::unique_ptr<Expression> get_reduced() const override;
 private:
  std::string id;
};

template <typename Iterator>
std::unique_ptr<Expression> Expression::create(
    parser::ConcreteSyntaxTree<Token> &source, Iterator &string_iterator) {
  // TODO
}

}

#endif