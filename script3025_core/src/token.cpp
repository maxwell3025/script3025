#include "token.hpp"

#include <iomanip>

std::ostream &script3025::operator<<(std::ostream &os, const script3025::Token &token) {
  switch (token) {
  case script3025::Token::COLON:
    os << ":";
    break;
  case script3025::Token::PERIOD:
    os << ".";
    break;
  case script3025::Token::LET:
    os << "let";
    break;
  case script3025::Token::DEF:
    os << ":=";
    break;
  case script3025::Token::INDUCTIVE:
    os << "inductive";
    break;
  case script3025::Token::ID:
    os << "i";
    break;
  case script3025::Token::NUMBER:
    os << "n";
    break;
  case script3025::Token::LAMBDA:
    os << "λ";
    break;
  case script3025::Token::PI:
    os << "Π";
    break;
  case script3025::Token::L_PAREN:
    os << "(";
    break;
  case script3025::Token::R_PAREN:
    os << ")";
    break;
  case script3025::Token::ABS:
    os << "A";
    break;
  case script3025::Token::EXPR_PAREN:
    os << "E₂";
    break;
  case script3025::Token::EXPR_APP:
    os << "E₁";
    break;
  case script3025::Token::EXPR_ABS:
    os << "E₀";
    break;
  case script3025::Token::DEFN:
    os << "D";
    break;
  case script3025::Token::IDEFN:
    os << "I";
    break;
  case script3025::Token::PROG:
    os << "P";
    break;
  case script3025::Token::END:
    os << "$";
    break;
  default:
    os << "<" << std::hex << std::setw(2) << std::setfill('0') << (int)token <<
          ">";
    break;
  }
  return os;
}
