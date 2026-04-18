#ifndef SCRIPT3025_SCRIPT3025_CORE_EQUALITY_VISITOR_HPP
#define SCRIPT3025_SCRIPT3025_CORE_EQUALITY_VISITOR_HPP

#include <memory>
#include <unordered_map>

#include "expression/expression_base.hpp"
#include "expression/expression_visitor.hpp"
#include "spdlog/common.h"
#include "spdlog/logger.h"
#include "spdlog/sinks/stdout_color_sinks-inl.h"

namespace script3025 {

class EqualityVisitor : public ConstExpressionVisitor {
 public:
  EqualityVisitor(const Expression *lhs);
  void visit_id(const IdExpression &rhs) override;
  void visit_nat_literal(const NatLiteralExpression &rhs) override;
  void visit_type_keyword(const TypeKeywordExpression &rhs) override;
  [[nodiscard]] bool get() const;

 protected:
  void visit_expression(const Expression &e) override;

 private:
  bool unequal_;
  const Expression *lhs_;
  std::unordered_map<const Expression *, const Expression *> pointer_map_;

  std::shared_ptr<spdlog::logger> get_logger() {
    static std::shared_ptr<spdlog::logger> logger =
        ([&]() -> std::shared_ptr<spdlog::logger> {
          logger = spdlog::stderr_color_mt("script3025::EqualityVisitor",
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
