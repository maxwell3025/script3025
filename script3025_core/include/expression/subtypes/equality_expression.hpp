#ifndef SCRIPT3025_SCRIPT3025_CORE_EQUALITY_EXPRESSION_HPP
#define SCRIPT3025_SCRIPT3025_CORE_EQUALITY_EXPRESSION_HPP

#include <memory>

#include "expression/expression_base.hpp"
#include "expression/expression_visitor.hpp"
#include "spdlog/logger.h"

namespace script3025 {

class EqualityExpression : public Expression {
 public:
  EqualityExpression(std::unique_ptr<Expression> &&lhs,
                     std::unique_ptr<Expression> &&rhs);
  EqualityExpression();
  void accept(ExpressionVisitor<true> &visitor) const override {
    visitor.visit_equality(*this);
  }
  void accept(ExpressionVisitor<false> &visitor) override {
    visitor.visit_equality(*this);
  }
  [[nodiscard]] std::unique_ptr<Expression> &lhs() { return children[0]; }
  [[nodiscard]] std::unique_ptr<Expression> &rhs() { return children[1]; }
  [[nodiscard]] const std::unique_ptr<Expression> &lhs() const {
    return children[0];
  }
  [[nodiscard]] const std::unique_ptr<Expression> &rhs() const {
    return children[1];
  }

 private:
  [[nodiscard]] static std::shared_ptr<spdlog::logger> get_logger();
};

}  // namespace script3025

#endif
