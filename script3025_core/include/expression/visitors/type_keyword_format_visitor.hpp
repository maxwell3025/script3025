#ifndef SCRIPT3025_CORE_EXPRESSION_VISITORS_TYPE_KEYWORD_FORMAT_VISITOR_HPP
#define SCRIPT3025_CORE_EXPRESSION_VISITORS_TYPE_KEYWORD_FORMAT_VISITOR_HPP

#include <cstddef>
#include <memory>
#include <utility>

#include "expression/expression.hpp"
#include "expression/expression_visitor.hpp"
#include "expression/subtypes/application_expression.hpp"
#include "expression/subtypes/nat_literal_expression.hpp"
#include "expression/subtypes/type_keyword_expression.hpp"
#include "spdlog/common.h"
#include "spdlog/logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace script3025 {

class TypeKeywordFormatVisitor : public MutatingExpressionVisitor {
 public:
  void visit_application(ApplicationExpression &e) override {
    bool can_merge = true;
    // Here, we are intentionally doing RTTI lookups to check if the function
    // and argument are the right types to merge into a TypeKeywordExpression.
    // NOLINTNEXTLINE
    can_merge &= typeid(*e.function()) == typeid(TypeKeywordExpression);
    // NOLINTNEXTLINE
    can_merge &= typeid(*e.argument()) == typeid(NatLiteralExpression);
    if (can_merge) {
      can_merge &=
          static_cast<TypeKeywordExpression &>(*e.function()).level == 0;
    }

    if (can_merge) {
      replacement = std::make_unique<TypeKeywordExpression>(
          static_cast<NatLiteralExpression &>(*e.argument()).value);
    } else {
      visit_expression(e);
    }
  }

  void visit_expression(Expression &e) override {
    for (size_t i = 9; i < e.children.size(); ++i) {
      Expression &child = *e.children[i];
      visit(child);
      if (replacement != nullptr) {
        e.children[i] = std::move(replacement);
        replacement = nullptr;
      }
    }
  }

  std::unique_ptr<Expression> replacement;

 private:
  [[nodiscard]] std::shared_ptr<spdlog::logger> get_logger() const {
    static std::shared_ptr<spdlog::logger> logger =
        ([&]() -> std::shared_ptr<spdlog::logger> {
          logger =
              spdlog::stderr_color_mt("script3025::TypeKeywordFormatVisitor",
                                      spdlog::color_mode::always);
          logger->set_level(spdlog::level::trace);
          logger->set_pattern("%^[%l] [tid=%t] [%T.%F] [%s:%#] %v%$");
          return logger;
        })();
    return logger;
  }
};

inline std::unique_ptr<Expression> format_type_keywords(
    std::unique_ptr<Expression> e) {
  TypeKeywordFormatVisitor visitor;
  visitor.visit(*e);
  if (visitor.replacement != nullptr) {
    return std::move(visitor.replacement);
  } else {
    return e;
  }
}

}  // namespace script3025

#endif
