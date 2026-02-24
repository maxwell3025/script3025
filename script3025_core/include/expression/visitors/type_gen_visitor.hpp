#ifndef SCRIPT3025_SCRIPT3025_CORE_TYPE_GEN_VISITOR
#define SCRIPT3025_SCRIPT3025_CORE_TYPE_GEN_VISITOR
#include <memory>
#include <unordered_map>

#include "expression/expression_base.hpp"
#include "expression/expression_visitor.hpp"
#include "expression/variable_reference.hpp"
#include "spdlog/sinks/stdout_color_sinks.h"
namespace script3025 {
class TypeGenVisitor : public ConstExpressionVisitor {
 public:
  TypeGenVisitor(
      std::unordered_map<Expression *, Expression *> expression_type_map,
      std::unordered_map<VariableReference, Expression *> variable_type_map);

  virtual void visit_induction_keyword(const InductionKeywordExpression &);
  virtual void visit_nat_keyword(const NatKeywordExpression &);
  virtual void visit_replace_keyword(const ReplaceKeywordExpression &);
  virtual void visit_reflexive_keyword(const ReflexiveKeywordExpression &);
  virtual void visit_succ_keyword(const SuccKeywordExpression &);
  virtual void visit_type_keyword(const TypeKeywordExpression &);
  virtual void visit_application(const ApplicationExpression &);
  virtual void visit_equality(const EqualityExpression &);
  virtual void visit_id(const IdExpression &);
  virtual void visit_nat_literal(const NatLiteralExpression &);

 protected:
  void visit_expression(const Expression &);

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
  }
};
}  // namespace script3025
#endif