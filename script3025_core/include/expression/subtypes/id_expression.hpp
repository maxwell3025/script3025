#ifndef SCRIPT3025_SCRIPT3025_CORE_ID_EXPRESSION_HPP
#define SCRIPT3025_SCRIPT3025_CORE_ID_EXPRESSION_HPP

#include <memory>
#include <string>

#include "expression/expression_base.hpp"
#include "expression/expression_visitor.hpp"
#include "spdlog/logger.h"

namespace script3025 {

class IdExpression : public Expression {
 public:
  IdExpression(std::string id, Expression *source);
  IdExpression();
  void accept(ExpressionVisitor<true> &visitor) const override {
    visitor.visit_id(*this);
  }
  void accept(ExpressionVisitor<false> &visitor) override {
    visitor.visit_id(*this);
  }

  std::string id;
  Expression *source;

 private:
  [[nodiscard]] static std::shared_ptr<spdlog::logger> get_logger();
};

}  // namespace script3025

#endif
