#include "token.hpp"

#include <iomanip>

namespace script3025 {

std::ostream &operator<<(std::ostream &os, const Token &token) {
  switch (token) {
  case Token::COLON:
    os << ":";
    break;
  case Token::PERIOD:
    os << ".";
    break;
  case Token::LET:
    os << "let";
    break;
  case Token::DEF:
    os << "def";
    break;
  case Token::ASSIGN:
    os << ":=";
    break;
  case Token::INDUCTIVE:
    os << "inductive";
    break;
  case Token::ID:
    os << "i";
    break;
  case Token::NUMBER:
    os << "n";
    break;
  case Token::EQ:
    os << "=";
    break;
  case Token::LAMBDA:
    os << "λ";
    break;
  case Token::PI:
    os << "Π";
    break;
  case Token::L_PAREN:
    os << "(";
    break;
  case Token::R_PAREN:
    os << ")";
    break;
  case Token::ABS:
    os << "A";
    break;
  case Token::EXPR_PAREN:
    os << "E₃";
    break;
  case Token::EXPR_APP:
    os << "E₂";
    break;
  case Token::EXPR_ABS:
    os << "E₁";
    break;
  case Token::EXPR_EQ:
    os << "E₀";
    break;
  case Token::DEFN:
    os << "D";
    break;
  case Token::PROG:
    os << "P";
    break;
  case Token::END:
    os << "$";
    break;
  default:
    os << "<" << std::hex << std::setw(2) << std::setfill('0') << (int)token <<
          ">";
    break;
  }
  return os;
}

} // namespace script3025
