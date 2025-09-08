#ifndef SCRIPT3025_SCRIPT3025_CORE_LAZY_REDUCTION_VISITOR
#define SCRIPT3025_SCRIPT3025_CORE_LAZY_REDUCTION_VISITOR

#include <memory>
#include <utility>

#include "expression/expression_base.hpp"
#include "expression/expression_visitor.hpp"
#include "spdlog/logger.h"

namespace script3025 {

// @brief
// This is just a naive reducer.
// This just rips away at alpha/beta reductions without looking at type labels.
// To implement safe reduction, first type check, reduce those type labels, and
// verify syntactic equality (possibly modulo eta expansions).
// Note that this should be considered to destroy the original expression.
class LazyReductionVisitor : public MutatingExpressionVisitor {
 public:
  void visit_application(ApplicationExpression &e) override;
  void visit_id(IdExpression &e) override;
  [[nodiscard]] std::unique_ptr<Expression> get() {
    return std::move(reduced_expression_);
  }

 protected:
  void visit_expression(Expression &e) override;

 private:
  [[nodiscard]] static std::shared_ptr<spdlog::logger> get_logger();

  std::unique_ptr<Expression> reduced_expression_;
};

inline std::unique_ptr<Expression> reduce(Expression &e) {
  LazyReductionVisitor visitor;
  e.accept(visitor);
  return visitor.get();
}

}  // namespace script3025

#endif
