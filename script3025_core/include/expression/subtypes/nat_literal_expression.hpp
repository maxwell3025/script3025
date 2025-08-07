#ifndef SCRIPT3025_SCRIPT3025_CORE_NAT_LITERAL_EXPRESSION_HPP
#define SCRIPT3025_SCRIPT3025_CORE_NAT_LITERAL_EXPRESSION_HPP

#include <gmpxx.h>

#include <memory>

#include "expression/expression_base.hpp"
#include "expression/expression_visitor.hpp"
#include "spdlog/logger.h"

namespace script3025 {

class NatLiteralExpression : public Expression {
 public:
  NatLiteralExpression(mpz_class value);
  NatLiteralExpression();
  void accept(ExpressionVisitor<true> &visitor) const override {
    visitor.visit_nat_literal(*this);
  }
  void accept(ExpressionVisitor<false> &visitor) override {
    visitor.visit_nat_literal(*this);
  }

  mpz_class value_;

 private:
  [[nodiscard]] static std::shared_ptr<spdlog::logger> get_logger();
};

}  // namespace script3025

#endif
