#include "parsing_utility.hpp"

#include <algorithm>
#include <cstddef>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "parser.hpp"
#include "spdlog/common.h"
#include "spdlog/logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
#include "token.hpp"

namespace script3025 {

namespace {

[[nodiscard]] bool is_alphanumeric(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

}  // namespace

std::vector<AnnotatedToken> tokenize(const std::string& text) {
  static std::shared_ptr<spdlog::logger> logger =
      ([&]() -> std::shared_ptr<spdlog::logger> {
        logger = spdlog::stderr_color_mt("script3025::tokenize",
                                         spdlog::color_mode::always);
        logger->set_level(spdlog::level::warn);
        logger->set_pattern("%^[%l] [tid=%t] [%T.%F] [%s:%#] %v%$");
        return logger;
      })();

  std::vector<AnnotatedToken> annotated_tokens;
  size_t i = 0;
  while (i < text.size()) {
    const size_t start = i;
    AnnotatedToken new_token;

    // Recognize numbers
    while (i < text.size() && text[i] >= '0' && text[i] <= '9') {
      ++i;
    }
    if (i > start) {
      new_token.text = text.substr(start, i - start);
      new_token.token = Token::NUMBER;
      annotated_tokens.push_back(new_token);
      continue;
    }

    // Recognize keywords
    if (text.substr(i, 9) == "inductive" &&
        !(text.length() > i + 9 && is_alphanumeric(text[i + 9]))) {
      i += 9;
      new_token.text = text.substr(start, i - start);
      new_token.token = Token::INDUCTIVE;
      annotated_tokens.push_back(new_token);
      continue;
    }

    if (text.substr(i, 6) == "lambda" &&
        !(text.length() > i + 6 && is_alphanumeric(text[i + 6]))) {
      i += 6;
      new_token.text = text.substr(start, i - start);
      new_token.token = Token::LAMBDA;
      annotated_tokens.push_back(new_token);
      continue;
    }

    if (text.substr(i, 3) == "let" &&
        !(text.length() > i + 3 && is_alphanumeric(text[i + 3]))) {
      i += 3;
      new_token.text = text.substr(start, i - start);
      new_token.token = Token::LET;
      annotated_tokens.push_back(new_token);
      continue;
    }

    if (text.substr(i, 3) == "def" &&
        !(text.length() > i + 3 && is_alphanumeric(text[i + 3]))) {
      i += 3;
      new_token.text = text.substr(start, i - start);
      new_token.token = Token::DEF;
      annotated_tokens.push_back(new_token);
      continue;
    }

    if (text.substr(i, 2) == "in" &&
        !(text.length() > i + 2 && is_alphanumeric(text[i + 2]))) {
      i += 2;
      new_token.text = text.substr(start, i - start);
      new_token.token = Token::IN;
      annotated_tokens.push_back(new_token);
      continue;
    }

    if (text.substr(i, 2) == "pi" &&
        !(text.length() > i + 2 && is_alphanumeric(text[i + 2]))) {
      i += 2;
      new_token.text = text.substr(start, i - start);
      new_token.token = Token::PI;
      annotated_tokens.push_back(new_token);
      continue;
    }

    if (text.substr(i, 2) == ":=") {
      i += 2;
      new_token.text = text.substr(start, i - start);
      new_token.token = Token::ASSIGN;
      annotated_tokens.push_back(new_token);
      continue;
    }

    // Recognize identifiers
    while (i < text.size() && is_alphanumeric(text[i])) {
      ++i;
    }
    if (i > start) {
      new_token.text = text.substr(start, i - start);
      new_token.token = Token::ID;
      annotated_tokens.push_back(new_token);
      continue;
    }

    // Recognize symbols
    ++i;
    switch (text[start]) {
      case '(':
        new_token.text = text.substr(start, i - start);
        new_token.token = Token::L_PAREN;
        annotated_tokens.push_back(new_token);
        continue;
      case ')':
        new_token.text = text.substr(start, i - start);
        new_token.token = Token::R_PAREN;
        annotated_tokens.push_back(new_token);
        continue;
      case '.':
        new_token.text = text.substr(start, i - start);
        new_token.token = Token::PERIOD;
        annotated_tokens.push_back(new_token);
        continue;
      case ':':
        new_token.text = text.substr(start, i - start);
        new_token.token = Token::COLON;
        annotated_tokens.push_back(new_token);
        continue;
      case '=':
        new_token.text = text.substr(start, i - start);
        new_token.token = Token::EQ;
        annotated_tokens.push_back(new_token);
        continue;
      case ' ':
      case '\t':
      case '\n':
        continue;
      default:
        SPDLOG_LOGGER_ERROR(logger, "invalid character: \"{}\"", text[i]);
    }
  }

  std::stringstream message;
  for (const AnnotatedToken& annotated_token : annotated_tokens) {
    message << annotated_token.token;
  }
  SPDLOG_LOGGER_TRACE(logger, "{}", message.str());
  return annotated_tokens;
}

ParsedCode parse(const std::string& text) {
  static std::shared_ptr<spdlog::logger> logger =
      ([&]() -> std::shared_ptr<spdlog::logger> {
        logger = spdlog::stderr_color_mt("script3025::parse",
                                         spdlog::color_mode::always);
        logger->set_level(spdlog::level::warn);
        logger->set_pattern("%^[%l] [tid=%t] [%T.%F] [%s:%#] %v%$");
        return logger;
      })();

  static const std::unique_ptr<parser::Parser<Token>> basic_parser =
      std::make_unique<parser::Parser<Token>>(
          parser::ParserBuilder<Token>(Token::PROG, Token::END)
              .rule(Token::PROG)
              .rule(Token::PROG, Token::PROG, Token::DEFN)
              .rule(Token::DEFN, Token::DEF, Token::ID, Token::ASSIGN,
                    Token::EXPR_ABS)
              // This is a horrifying bag of worms I don't want to touch!
              // .rule(Token::IDEFN, Token::INDUCTIVE, Token::ID, Token::ASSIGN,
              //       Token::EXPR_ABS)

              .rule(Token::ABS, Token::LAMBDA, Token::L_PAREN, Token::ID,
                    Token::COLON, Token::EXPR_ABS, Token::R_PAREN,
                    Token::PERIOD, Token::EXPR_ABS)
              .rule(Token::ABS, Token::LAMBDA, Token::ID, Token::COLON,
                    Token::EXPR_ABS, Token::PERIOD, Token::EXPR_ABS)
              .rule(Token::ABS, Token::PI, Token::L_PAREN, Token::ID,
                    Token::COLON, Token::EXPR_ABS, Token::R_PAREN,
                    Token::PERIOD, Token::EXPR_ABS)
              .rule(Token::ABS, Token::PI, Token::ID, Token::COLON,
                    Token::EXPR_ABS, Token::PERIOD, Token::EXPR_ABS)
              .rule(Token::ABS, Token::LET, Token::L_PAREN, Token::ID,
                    Token::COLON, Token::EXPR_ABS, Token::R_PAREN,
                    Token::ASSIGN, Token::EXPR_ABS, Token::IN, Token::EXPR_ABS)
              .rule(Token::ABS, Token::LET, Token::ID, Token::COLON,
                    Token::EXPR_ABS, Token::ASSIGN, Token::EXPR_ABS, Token::IN,
                    Token::EXPR_ABS)

              .rule(Token::EXPR_EQ, Token::EXPR_ABS, Token::EQ, Token::EXPR_ABS)

              .rule(Token::EXPR_ABS, Token::ABS)
              .rule(Token::EXPR_ABS, Token::EXPR_APP, Token::ABS)
              .rule(Token::EXPR_ABS, Token::EXPR_APP)

              .rule(Token::EXPR_APP, Token::EXPR_APP, Token::EXPR_PAREN)
              .rule(Token::EXPR_APP, Token::EXPR_PAREN)

              .rule(Token::EXPR_PAREN, Token::ID)
              .rule(Token::EXPR_PAREN, Token::NUMBER)
              .rule(Token::EXPR_PAREN, Token::L_PAREN, Token::EXPR_ABS,
                    Token::R_PAREN)

              .build());

  ParsedCode output;
  output.annotated_tokens = tokenize(text);

  std::vector<Token> token_list(output.annotated_tokens.size());
  std::transform(output.annotated_tokens.begin(), output.annotated_tokens.end(),
                 token_list.begin(), [](AnnotatedToken d) { return d.token; });

  output.cst = std::make_unique<parser::ConcreteSyntaxTree<Token>>(
      basic_parser->parse(token_list.begin(), token_list.end()));

  SPDLOG_LOGGER_TRACE(logger, "Generated tree:\n{}", output.cst->to_string());
  return output;
}

}  // namespace script3025
