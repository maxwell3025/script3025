#ifndef SCRIPT3025_SCRIPT3025_CORE_LET_EXPRESSION_HPP
#define SCRIPT3025_SCRIPT3025_CORE_LET_EXPRESSION_HPP

#include "expression/expression_base.hpp"
#include "expression/expression_visitor.hpp"

namespace script3025 {

class LetExpression : public Expression {
 public:
  LetExpression(std::string argument_id,
                std::unique_ptr<Expression> &&argument_type,
                std::unique_ptr<Expression> &&argument_value,
                std::unique_ptr<Expression> &&definition);
  LetExpression();
  void accept(ExpressionVisitor &visitor) const override;
  void accept(MutatingExpressionVisitor &visitor) override;
  constexpr std::unique_ptr<Expression>& argument_type() { return children[0]; }
  constexpr std::unique_ptr<Expression>& argument_value() { return children[1]; }
  constexpr std::unique_ptr<Expression>& definition() { return children[2]; }

  std::string argument_id;

 private:
  static std::shared_ptr<spdlog::logger> get_logger();
};

} // namespace script3025

#endif
