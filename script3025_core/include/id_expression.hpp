#ifndef SCRIPT3025_SCRIPT3025_CORE_ID_EXPRESSION_HPP
#define SCRIPT3025_SCRIPT3025_CORE_ID_EXPRESSION_HPP

#include "expression.hpp"
#include "expression_visitor.hpp"

namespace script3025 {

class IdExpression : public Expression {
 public:
  IdExpression(std::string &&id, Expression *source);
  IdExpression(const IdExpression &other);
  std::unique_ptr<Expression> clone() const override;
  bool is_normal() const override;
  Expression *get_type() override;
  std::unique_ptr<Expression> replace(const std::string &id,
                                      const Expression *source,
                                      const Expression &expression) override;
  std::unique_ptr<Expression> reduce() override;
  bool operator==(const Expression &other) const override;
  std::ostream &print(std::ostream &os) const override;
  void accept(ExpressionVisitor &visitor) const override;
  void accept(MutatingExpressionVisitor &visitor) override;

 private:
  std::string id;
  Expression *source;
  static std::shared_ptr<spdlog::logger> get_logger();
};

} // namespace script3025

#endif
