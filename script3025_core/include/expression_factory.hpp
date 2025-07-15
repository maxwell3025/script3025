#ifndef SCRIPT3025_SCRIPT3025_CORE_EXPRESSION_FACTORY_HPP
#define SCRIPT3025_SCRIPT3025_CORE_EXPRESSION_FACTORY_HPP

#include "expression/expression.hpp"
#include "expression/subtypes/application_expression.hpp"
#include "expression/subtypes/id_expression.hpp"
#include "expression/subtypes/lambda_expression.hpp"
#include "expression/subtypes/let_expression.hpp"
#include "expression/subtypes/pi_expression.hpp"

namespace script3025 {

template <typename Iterator>
std::unique_ptr<Expression> create_expression(
    const parser::ConcreteSyntaxTree<Token> &source,
    Iterator &string_iterator) {
  static std::shared_ptr<spdlog::logger> logger =
      ([&] () -> std::shared_ptr<spdlog::logger> {
        logger = spdlog::stderr_color_mt("script3025::create_expression",
                                         spdlog::color_mode::always);
        logger -> set_level(spdlog::level::warn);
        logger -> set_pattern("%^[%l] [tid=%t] [%T.%F] [%s:%#] %v%$");
        return logger;
      })();

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
    std::unique_ptr<Expression> argument_type = create_expression(
        source.children[4], string_iterator);

    // )
    ++string_iterator;

    // .
    ++string_iterator;

    // definition
    std::unique_ptr<Expression> definition = create_expression(
        source.children[7], string_iterator);

    return std::make_unique<LambdaExpression>(std::move(identifier),
        std::move(argument_type), std::move(definition));
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
    std::unique_ptr<Expression> argument_type = create_expression(
        source.children[3], string_iterator);

    // .
    ++string_iterator;

    // definition
    std::unique_ptr<Expression> definition = create_expression(
        source.children[5], string_iterator);

    return std::make_unique<LambdaExpression>(std::move(identifier),
        std::move(argument_type), std::move(definition));
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
    std::unique_ptr<Expression> argument_type = create_expression(
        source.children[4], string_iterator);

    // )
    ++string_iterator;

    // .
    ++string_iterator;

    // definition
    std::unique_ptr<Expression> definition = create_expression(
        source.children[7], string_iterator);

    return std::make_unique<PiExpression>(std::move(identifier),
        std::move(argument_type), std::move(definition));
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
    std::unique_ptr<Expression> argument_type = create_expression(
        source.children[3], string_iterator);

    // .
    ++string_iterator;

    // definition
    std::unique_ptr<Expression> definition = create_expression(
        source.children[5], string_iterator);

    return std::make_unique<PiExpression>(std::move(identifier),
        std::move(argument_type), std::move(definition));
  } else if (sentential_form == std::vector({Token::L_PAREN, Token::EXPR,
                                            Token::R_PAREN})) {
    // (
    ++string_iterator;

    // expression
    std::unique_ptr<Expression> inner_expression = create_expression(
        source.children[1], string_iterator);

    // )
    ++string_iterator;

    return inner_expression;
  } else if (sentential_form == std::vector({Token::EXPR, Token::EXPR})) {
    std::unique_ptr<Expression> function = create_expression(
        source.children[0], string_iterator);

    std::unique_ptr<Expression> argument = create_expression(
        source.children[1], string_iterator);

    return std::make_unique<ApplicationExpression>(std::move(function),
                                                   std::move(argument));
  } else if (sentential_form == std::vector({Token::ID})) {
    std::string id(*string_iterator);
    ++string_iterator;
    return std::make_unique<IdExpression>(std::move(id), nullptr);
  } else {
    std::stringstream sentential_form_string;
    for (const Token &token : sentential_form) {
      sentential_form_string << token;
    }
    SPDLOG_LOGGER_ERROR(
        logger,
        "This sentential form failed to match any known expressions: {}",
        sentential_form_string.str());
    throw std::runtime_error("Failed to match token sequence.");
  }
}

} // namespace script3025

#endif
