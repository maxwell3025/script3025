#ifndef SCRIPT3025_SCRIPT3025_CORE_ID_EXPRESSION_HPP
#define SCRIPT3025_SCRIPT3025_CORE_ID_EXPRESSION_HPP

#include "expression/expression_base.hpp"
#include "expression/expression_visitor.hpp"

namespace script3025 {

class IdExpression : public Expression {
 public:
  IdExpression(std::string id, Expression *source);
  IdExpression();
  void accept(ExpressionVisitor &visitor) const override;
  void accept(MutatingExpressionVisitor &visitor) override;

  std::string id;
  Expression *source;

 private:
  static std::shared_ptr<spdlog::logger> get_logger();
};

}  // namespace script3025

#endif
