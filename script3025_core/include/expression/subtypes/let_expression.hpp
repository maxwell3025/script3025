#ifndef SCRIPT3025_SCRIPT3025_CORE_LET_EXPRESSION_HPP
#define SCRIPT3025_SCRIPT3025_CORE_LET_EXPRESSION_HPP

#include <memory>
#include <string>

#include "expression/expression_visitor.hpp"
#include "scope_expression.hpp"
#include "spdlog/logger.h"

namespace script3025 {

class LetExpression : public ScopeExpression {
 public:
  LetExpression(std::string argument_id,
                std::unique_ptr<Expression>&& argument_type,
                std::unique_ptr<Expression>&& argument_value,
                std::unique_ptr<Expression>&& definition);
  LetExpression();
  void accept(ExpressionVisitor<true>& visitor) const override {
    visitor.visit_let(*this);
  }
  void accept(ExpressionVisitor<false>& visitor) override {
    visitor.visit_let(*this);
  }
  [[nodiscard]] std::unique_ptr<Expression>& argument_type() override {
    return children[0];
  }
  [[nodiscard]] std::unique_ptr<Expression>& argument_value() {
    return children[1];
  }
  [[nodiscard]] std::unique_ptr<Expression>& definition() override {
    return children[2];
  }
  [[nodiscard]] const std::unique_ptr<Expression>& argument_type()
      const override {
    return children[0];
  }
  [[nodiscard]] const std::unique_ptr<Expression>& argument_value() const {
    return children[1];
  }
  [[nodiscard]] const std::unique_ptr<Expression>& definition() const override {
    return children[2];
  }

 private:
  [[nodiscard]] static std::shared_ptr<spdlog::logger> get_logger();
};

}  // namespace script3025

#endif
