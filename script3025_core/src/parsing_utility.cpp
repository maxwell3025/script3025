#include <iostream>

#include "parsing_utility.hpp"

namespace script3025 {

bool is_alphanumeric(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

std::unique_ptr<parser::Parser<Token>> basic_parser;

ParsedCode parse(std::string text) {
  static std::shared_ptr<spdlog::logger> logger =
      ([&] () -> std::shared_ptr<spdlog::logger> {
        logger = spdlog::stderr_color_mt("script3025::parse", spdlog::color_mode::always);
        logger-> set_level(spdlog::level::warn);
        logger-> set_pattern("%^[%l] [tid=%t] [%T.%F] [%s:%#] %v%$");
        return logger;
      })();
  // Tokenize the text
  ParsedCode output;
  std::vector<Token> token_sequence;
  size_t i = 0;
  while (i < text.size()) {
    size_t start = i;

    // Recognize numbers
    while (i < text.size() && text[i] >= '0' && text[i] <= '9') {
      i++;
    }
    if (i > start) {
      token_sequence.push_back(Token::NUMBER);
      output.segment_begin.push_back(start);
      output.segment_end.push_back(i);
      continue;
    }

    // Recognize keywords
    if (text.substr(i, 9) == "inductive" && !(text.length() > i + 9 && is_alphanumeric(text[i + 9]))) {
      i += 9;
      token_sequence.push_back(Token::INDUCTIVE);
      output.segment_begin.push_back(start);
      output.segment_end.push_back(i);
      continue;
    }

    if (text.substr(i, 6) == "lambda" && !(text.length() > i + 6 && is_alphanumeric(text[i + 6]))) {
      i += 6;
      token_sequence.push_back(Token::LAMBDA);
      output.segment_begin.push_back(start);
      output.segment_end.push_back(i);
      continue;
    }

    if (text.substr(i, 3) == "let" && !(text.length() > i + 3 && is_alphanumeric(text[i + 3]))) {
      i += 3;
      token_sequence.push_back(Token::LET);
      output.segment_begin.push_back(start);
      output.segment_end.push_back(i);
      continue;
    }

    if (text.substr(i, 2) == "pi" && !(text.length() > i + 2 && is_alphanumeric(text[i + 2]))) {
      i += 2;
      token_sequence.push_back(Token::PI);
      output.segment_begin.push_back(start);
      output.segment_end.push_back(i);
      continue;
    }

    if (text.substr(i, 2) == ":=") {
      i += 2;
      token_sequence.push_back(Token::DEF);
      output.segment_begin.push_back(start);
      output.segment_end.push_back(i);
      continue;
    }

    // Recognize identifiers
    while (
        i < text.size() && is_alphanumeric(text[i])) {
      ++i;
    }
    if (i > start) {
      token_sequence.push_back(Token::ID);
      output.segment_begin.push_back(start);
      output.segment_end.push_back(i);
      continue;
    }

    // Recognize symbols
    switch (text[i]) {
    case '(':
      token_sequence.push_back(Token::L_PAREN);
      output.segment_begin.push_back(i);
      output.segment_end.push_back(++i);
      continue;
    case ')':
      token_sequence.push_back(Token::R_PAREN);
      output.segment_begin.push_back(i);
      output.segment_end.push_back(++i);
      continue;
    case '.':
      token_sequence.push_back(Token::PERIOD);
      output.segment_begin.push_back(i);
      output.segment_end.push_back(++i);
      continue;
    case ':':
      token_sequence.push_back(Token::COLON);
      output.segment_begin.push_back(i);
      output.segment_end.push_back(++i);
      continue;
    case ' ':
    case '\t':
    case '\n':
      ++i;
      continue;
    default:
      std::cerr << "invalid character: \"" << text[i] << std::endl;
    }
  }

  std::stringstream message;
  for (Token token : token_sequence) {
    message << token;
  }
  SPDLOG_LOGGER_TRACE(logger, "{}", message.str());

  if (!basic_parser) {
    basic_parser = std::make_unique<parser::Parser<Token>>(
        parser::ParserBuilder<Token>(Token::PROG, Token::END)
        .rule(Token::PROG)
        .rule(Token::PROG, Token::PROG, Token::DEFN)
        .rule(Token::DEFN, Token::LET, Token::ID, Token::DEF, Token::EXPR_ABS)
        // This is a horrifying bag of worms I don't want to touch!
        // .rule(Token::IDEFN, Token::INDUCTIVE, Token::ID, Token::DEF, Token::EXPR_ABS)

        .rule(Token::ABS, Token::LAMBDA, Token::L_PAREN, Token::ID,
              Token::COLON, Token::EXPR_ABS, Token::R_PAREN,Token::PERIOD,
              Token::EXPR_ABS)
        .rule(Token::ABS, Token::LAMBDA, Token::ID, Token::COLON,
              Token::EXPR_ABS, Token::PERIOD, Token::EXPR_ABS)
        .rule(Token::ABS, Token::PI, Token::L_PAREN, Token::ID, Token::COLON,
              Token::EXPR_ABS, Token::R_PAREN, Token::PERIOD, Token::EXPR_ABS)
        .rule(Token::ABS, Token::PI, Token::ID, Token::COLON,
              Token::EXPR_ABS, Token::PERIOD, Token::EXPR_ABS)

        .rule(Token::EXPR_ABS, Token::ABS)
        .rule(Token::EXPR_ABS, Token::EXPR_APP, Token::ABS)
        .rule(Token::EXPR_ABS, Token::EXPR_APP)

        .rule(Token::EXPR_APP, Token::EXPR_APP, Token::EXPR_PAREN)
        .rule(Token::EXPR_APP, Token::EXPR_PAREN)

        .rule(Token::EXPR_PAREN, Token::ID)
        .rule(Token::EXPR_PAREN, Token::L_PAREN, Token::EXPR_ABS, Token::R_PAREN)

        .build()
    );
  }

  output.cst = std::make_unique<parser::ConcreteSyntaxTree<Token>>(
      (*basic_parser).parse(token_sequence.begin(), token_sequence.end()));

  SPDLOG_LOGGER_TRACE(logger, "Generated tree:\n{}", output.cst -> to_string());
  output.text = std::move(text);
  return output;
}

}
