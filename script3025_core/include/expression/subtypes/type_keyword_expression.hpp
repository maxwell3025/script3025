#ifndef SCRIPT3025_SCRIPT3025_CORE_TYPE_KEYWORD_EXPRESSION_HPP
#define SCRIPT3025_SCRIPT3025_CORE_TYPE_KEYWORD_EXPRESSION_HPP

#include <gmpxx.h>

#include "expression/expression_visitor.hpp"
#include "expression/subtypes/keyword_expression.hpp"

namespace script3025 {

class TypeKeywordExpression : public KeywordExpression {
 public:
  explicit TypeKeywordExpression(mpz_class level);
  TypeKeywordExpression();
  void accept(ExpressionVisitor<true> &visitor) const override { visitor.visit_type_keyword(*this); }
  void accept(ExpressionVisitor<false> &visitor) override { visitor.visit_type_keyword(*this); }

  mpz_class level_;

 private:
  static std::shared_ptr<spdlog::logger> get_logger();
};

}  // namespace script3025

#endif
