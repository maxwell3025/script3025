#ifndef SCRIPT3025_SCRIPT3025_CORE_TYPE_GEN_VISITOR
#define SCRIPT3025_SCRIPT3025_CORE_TYPE_GEN_VISITOR
#include <memory>
#include <unordered_map>

#include "expression/expression_base.hpp"
#include "expression/expression_visitor.hpp"
#include "expression/variable_reference.hpp"
#include "spdlog/sinks/stdout_color_sinks.h"
namespace script3025 {
// This visitor generates non-reduced types for expressions.
// This visitor generates types in post-fix order, so each expression can assume all subexpressions are typed.
class TypeGenVisitor : public ConstExpressionVisitor {
 public:
  TypeGenVisitor(
      std::unordered_map<const Expression *, std::unique_ptr<Expression>> expression_type_map,
      std::unordered_map<VariableReference, std::unique_ptr<Expression>> variable_type_map);

  virtual void visit_lambda(const LambdaExpression &) override;
  virtual void visit_let(const LetExpression &) override;
  virtual void visit_pi(const PiExpression &) override;
  virtual void visit_induction_keyword(const InductionKeywordExpression &) override;
  virtual void visit_nat_keyword(const NatKeywordExpression &) override;
  virtual void visit_replace_keyword(const ReplaceKeywordExpression &) override;
  virtual void visit_reflexive_keyword(const ReflexiveKeywordExpression &) override;
  virtual void visit_succ_keyword(const SuccKeywordExpression &) override;
  virtual void visit_type_keyword(const TypeKeywordExpression &) override;
  virtual void visit_application(const ApplicationExpression &) override;
  virtual void visit_equality(const EqualityExpression &) override;
  virtual void visit_id(const IdExpression &) override;
  virtual void visit_nat_literal(const NatLiteralExpression &) override;

 private:
  // Maps expressions to their corresponding types.
  std::unordered_map<const Expression *, std::unique_ptr<Expression>>
      expression_type_map_;
  // Maps variables to their corresponding types.
  std::unordered_map<VariableReference, std::unique_ptr<Expression>>
      variable_type_map_;

  std::shared_ptr<spdlog::logger> get_logger() {
    static std::shared_ptr<spdlog::logger> logger =
        ([&]() -> std::shared_ptr<spdlog::logger> {
          logger = spdlog::stderr_color_mt("script3025::TypeGenVisitor",
                                           spdlog::color_mode::always);
          logger->set_level(spdlog::level::trace);
          logger->set_pattern("%^[%l] [tid=%t] [%T.%F] [%s:%#] %v%$");
          return logger;
        })();
    return logger;
  }
};
}  // namespace script3025
#endif