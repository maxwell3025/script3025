#ifndef SCRIPT3025_SCRIPT3025_CORE_LET_EXPRESSION_HPP
#define SCRIPT3025_SCRIPT3025_CORE_LET_EXPRESSION_HPP

#include "expression.hpp"
#include "expression_visitor.hpp"

namespace script3025 {

class LetExpression : public Expression {
 public:
  LetExpression(std::string &&argument_id,
                std::unique_ptr<Expression> &&argument_type,
                std::unique_ptr<Expression> &&argument_value,
                std::unique_ptr<Expression> &&definition);
  LetExpression(const LetExpression &other);
  LetExpression();
  bool is_normal() const override;
  std::ostream &print(std::ostream &os) const override;
  void accept(ExpressionVisitor &visitor) const override;
  void accept(MutatingExpressionVisitor &visitor) override;

  std::string argument_id;
  std::unique_ptr<Expression> argument_type;
  std::unique_ptr<Expression> argument_value;
  std::unique_ptr<Expression> definition;

 private:
  static std::shared_ptr<spdlog::logger> get_logger();
};

} // namespace script3025

#endif
