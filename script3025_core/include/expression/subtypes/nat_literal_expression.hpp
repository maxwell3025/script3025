#ifndef SCRIPT3025_SCRIPT3025_CORE_NAT_LITERAL_EXPRESSION_HPP
#define SCRIPT3025_SCRIPT3025_CORE_NAT_LITERAL_EXPRESSION_HPP

#include <gmpxx.h>

#include "expression/expression_base.hpp"
#include "expression/expression_visitor.hpp"

namespace script3025 {

class NatLiteralExpression : public Expression {
 public:
  NatLiteralExpression(mpz_class value);
  NatLiteralExpression();
  void accept(ExpressionVisitor &visitor) const override;
  void accept(MutatingExpressionVisitor &visitor) override;

  mpz_class value_;

 private:
  static std::shared_ptr<spdlog::logger> get_logger();
};

}  // namespace script3025

#endif
