#ifndef SCRIPT3025_SCRIPT3025_CORE_LAZY_REDUCTION_VISITOR
#define SCRIPT3025_SCRIPT3025_CORE_LAZY_REDUCTION_VISITOR

#include <memory>
#include <utility>

#include "expression/expression_base.hpp"
#include "expression/expression_visitor.hpp"
#include "expression/subtypes/nat_literal_expression.hpp"
#include "expression/variable_reference.hpp"
#include "spdlog/common.h"
#include "spdlog/logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

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
  void visit_nat_literal(NatLiteralExpression &e) override;
  [[nodiscard]] std::unique_ptr<Expression> get() {
    return std::move(reduced_expression_);
  }

  const std::unordered_map<VariableReference, const Expression *> *delta_table;

 protected:
  void visit_expression(Expression &e) override;

 private:
  [[nodiscard]] static std::shared_ptr<spdlog::logger> get_logger();

  std::unique_ptr<Expression> reduced_expression_;
};

inline std::unique_ptr<Expression> reduce(
    Expression &e,
    const std::unordered_map<VariableReference, const Expression *> *delta_table) {
  static std::shared_ptr<spdlog::logger> logger =
      ([&]() -> std::shared_ptr<spdlog::logger> {
        logger = spdlog::stderr_color_mt("script3025::reduce",
                                         spdlog::color_mode::always);
        logger->set_level(spdlog::level::trace);
        logger->set_pattern("%^[%l] [tid=%t] [%T.%F] [%s:%#] %v%$");
        return logger;
      })();
  
  SPDLOG_LOGGER_TRACE(logger, "Reducing {}", e);
  LazyReductionVisitor visitor;
  visitor.delta_table = delta_table;
  e.accept(visitor);
  return visitor.get();
}

}  // namespace script3025

#endif
