#ifndef SCRIPT3025_SCRIPT3025_CORE_TYPE_KEYWORD_EXPRESSION_HPP
#define SCRIPT3025_SCRIPT3025_CORE_TYPE_KEYWORD_EXPRESSION_HPP

#include <gmpxx.h>

#include "expression/expression_base.hpp"
#include "expression/expression_visitor.hpp"

namespace script3025 {

class TypeKeywordExpression : public Expression {
 public:
  explicit TypeKeywordExpression(mpz_class level);
  TypeKeywordExpression();
  void accept(ExpressionVisitor &visitor) const override;
  void accept(MutatingExpressionVisitor &visitor) override;

  mpz_class level_;

 private:
  static std::shared_ptr<spdlog::logger> get_logger();
};

} // namespace script3025

#endif
