#ifndef SCRIPT3025_SCRIPT3025_CORE_LAZY_REDUCTION_VISITOR
#define SCRIPT3025_SCRIPT3025_CORE_LAZY_REDUCTION_VISITOR

#include "expression/expression_base.hpp"
#include "expression/expression_visitor.hpp"

namespace script3025 {

class LazyReductionVisitor : public MutatingExpressionVisitor {
 public:
  void visit_application(ApplicationExpression &e) override;
  void visit_id(IdExpression &e) override;
  
  std::unique_ptr<Expression> reduced_expression;

 protected:
  void visit_expression(Expression &e);

 private:
  static std::shared_ptr<spdlog::logger> get_logger();
};

} // namespace script3025

#endif
