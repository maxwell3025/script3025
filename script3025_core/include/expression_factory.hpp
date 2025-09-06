#ifndef SCRIPT3025_SCRIPT3025_CORE_EXPRESSION_FACTORY_HPP
#define SCRIPT3025_SCRIPT3025_CORE_EXPRESSION_FACTORY_HPP

#include <iterator>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

#include "expression/expression.hpp"
#include "parser.hpp"
#include "spdlog/common.h"
#include "spdlog/logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
#include "token.hpp"

namespace script3025 {

template <typename Iterator>
[[nodiscard]] std::unique_ptr<Expression> create_expression(
    const parser::ConcreteSyntaxTree<Token> &source,
    Iterator &string_iterator) {
  static std::shared_ptr<spdlog::logger> logger =
      ([&]() -> std::shared_ptr<spdlog::logger> {
        logger = spdlog::stderr_color_mt("script3025::create_expression",
                                         spdlog::color_mode::always);
        logger->set_level(spdlog::level::warn);
        logger->set_pattern("%^[%l] [tid=%t] [%T.%F] [%s:%#] %v%$");
        return logger;
      })();

  std::vector<Token> sentential_form;
  std::transform(source.children.begin(), source.children.end(),
                 std::back_inserter(sentential_form),
                 [](const parser::ConcreteSyntaxTree<Token> &child) -> Token {
                   return child.symbol;
                 });

  if (sentential_form ==
      std::vector({Token::LAMBDA, Token::L_PAREN, Token::ID, Token::COLON,
                   Token::EXPR, Token::R_PAREN, Token::PERIOD, Token::EXPR})) {
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
    std::unique_ptr<Expression> argument_type =
        create_expression(source.children[4], string_iterator);

    // )
    ++string_iterator;

    // .
    ++string_iterator;

    // definition
    std::unique_ptr<Expression> definition =
        create_expression(source.children[7], string_iterator);

    return std::make_unique<LambdaExpression>(
        std::move(identifier), std::move(argument_type), std::move(definition));
  } else if (sentential_form ==
             std::vector({Token::LAMBDA, Token::ID, Token::COLON, Token::EXPR,
                          Token::PERIOD, Token::EXPR})) {
    // Lambda
    ++string_iterator;

    // Identifier
    std::string identifier = *string_iterator;
    ++string_iterator;

    // :
    ++string_iterator;

    // type bound
    std::unique_ptr<Expression> argument_type =
        create_expression(source.children[3], string_iterator);

    // .
    ++string_iterator;

    // definition
    std::unique_ptr<Expression> definition =
        create_expression(source.children[5], string_iterator);

    return std::make_unique<LambdaExpression>(
        std::move(identifier), std::move(argument_type), std::move(definition));
  } else if (sentential_form ==
             std::vector({Token::PI, Token::L_PAREN, Token::ID, Token::COLON,
                          Token::EXPR, Token::R_PAREN, Token::PERIOD,
                          Token::EXPR})) {
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
    std::unique_ptr<Expression> argument_type =
        create_expression(source.children[4], string_iterator);

    // )
    ++string_iterator;

    // .
    ++string_iterator;

    // definition
    std::unique_ptr<Expression> definition =
        create_expression(source.children[7], string_iterator);

    return std::make_unique<PiExpression>(
        std::move(identifier), std::move(argument_type), std::move(definition));
  } else if (sentential_form ==
             std::vector({Token::LAMBDA, Token::ID, Token::COLON, Token::EXPR,
                          Token::PERIOD, Token::EXPR})) {
    // Lambda
    ++string_iterator;

    // Identifier
    std::string identifier = *string_iterator;
    ++string_iterator;

    // :
    ++string_iterator;

    // type bound
    std::unique_ptr<Expression> argument_type =
        create_expression(source.children[3], string_iterator);

    // .
    ++string_iterator;

    // definition
    std::unique_ptr<Expression> definition =
        create_expression(source.children[5], string_iterator);

    return std::make_unique<PiExpression>(
        std::move(identifier), std::move(argument_type), std::move(definition));
  } else if (sentential_form ==
             std::vector({Token::L_PAREN, Token::EXPR, Token::R_PAREN})) {
    // (
    ++string_iterator;

    // expression
    std::unique_ptr<Expression> inner_expression =
        create_expression(source.children[1], string_iterator);

    // )
    ++string_iterator;

    return inner_expression;
  } else if (sentential_form == std::vector({Token::EXPR, Token::EXPR})) {
    std::unique_ptr<Expression> function =
        create_expression(source.children[0], string_iterator);

    std::unique_ptr<Expression> argument =
        create_expression(source.children[1], string_iterator);

    return std::make_unique<ApplicationExpression>(std::move(function),
                                                   std::move(argument));
  } else if (sentential_form ==
             std::vector({Token::EXPR, Token::EQ, Token::EXPR})) {
    std::unique_ptr<Expression> lhs =
        create_expression(source.children[0], string_iterator);

    std::unique_ptr<Expression> rhs =
        create_expression(source.children[1], string_iterator);

    return std::make_unique<EqualityExpression>(std::move(lhs), std::move(rhs));
  } else if (sentential_form == std::vector({Token::ID})) {
    std::string id = std::move(*string_iterator);
    ++string_iterator;
    if (id == "inductive") {
      return std::make_unique<InductionKeywordExpression>();
    } else if (id == "Nat") {
      return std::make_unique<NatKeywordExpression>();
    } else if (id == "refl") {
      return std::make_unique<ReflexiveKeywordExpression>();
    } else if (id == "subst") {
      return std::make_unique<ReplaceKeywordExpression>();
    } else if (id == "succ") {
      return std::make_unique<SuccKeywordExpression>();
    } else if (id == "Type") {
      return std::make_unique<TypeKeywordExpression>();
    } else {
      return std::make_unique<IdExpression>(std::move(id), nullptr);
    }
  } else if (sentential_form == std::vector({Token::NUMBER})) {
    std::string value = std::move(*string_iterator);
    ++string_iterator;
    // TODO
    // return std::make_unique<NatLiteralExpression>(std::move(id), nullptr);
    throw std::runtime_error(__FILE__ ": Unimplemented.");
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

}  // namespace script3025

#endif
