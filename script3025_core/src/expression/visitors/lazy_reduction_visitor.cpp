#include "expression/visitors/lazy_reduction_visitor.hpp"

#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <cstddef>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "expression/expression.hpp"
#include "expression/expression_visitor.hpp"
#include "expression/subtypes/application_expression.hpp"
#include "expression/subtypes/induction_keyword_expression.hpp"
#include "expression/subtypes/nat_literal_expression.hpp"
#include "expression/subtypes/reflexive_keyword_expression.hpp"
#include "expression/subtypes/replace_keyword_expression.hpp"
#include "expression/subtypes/succ_keyword_expression.hpp"
#include "expression/visitors/cloning_visitor.hpp"
#include "expression/visitors/replacing_visitor.hpp"
#include "partial_clone_visitor.hpp"

namespace script3025 {

class WHNFVisitor : public MutatingExpressionVisitor {
 public:
  void visit_application(ApplicationExpression &e) override {
    arguments.push_back(std::move(e.argument()));
    head = std::move(e.function());
    visit(*head);
  }

  void visit_equality(EqualityExpression &) override {
    if (arguments.size() != 0) {
      SPDLOG_LOGGER_WARN(
          get_logger(),
          "Attempting to reduce expression with Eq expression as a function:\n"
          "Eq expression:\n"
          "{}\n"
          "Expression:\n"
          "{}\n");
    }
  }

  void visit_id(IdExpression &e) override {
    auto replacement_it = delta_table->find(std::make_pair(e.id, e.source));
    if (replacement_it != delta_table->end()) {
      const Expression &replacement = *(replacement_it->second);
      CloningVisitor visitor;
      visitor.visit(replacement);
      head = visitor.get();
      visit(*head);
    }
  }

  void visit_lambda(LambdaExpression &e) override {
    ReplacingVisitor visitor(&e, e.argument_id, arguments.back().get());
    visitor.visit(*e.definition());
    arguments.pop_back();
    head = visitor.get();
    visit(*head);
  }

  void visit_let(LetExpression &e) override {
    ReplacingVisitor visitor(&e, e.argument_id, e.argument_value().get());
    visitor.visit(*e.definition());
    head = visitor.get();
    visit(*head);
  }

  void visit_pi(PiExpression &) override {
    if (arguments.size() != 0) {
      SPDLOG_LOGGER_WARN(
          get_logger(),
          "Attempting to reduce expression with Pi expression as a function:\n"
          "Pi expression:\n"
          "{}\n"
          "Expression:\n"
          "{}\n");
    }
  }

  void visit_replace_keyword(ReplaceKeywordExpression &) override {
    // Subject reduction is only conserved if the equality proof is a direct
    // reflexive proof.
    // Another perspective is that this prevents substitution using equality
    // proofs constructed by inducting through opaque symbols.
    if (arguments.size() < 6) return;
    arguments[arguments.size() - 5] =
        reduce(*arguments[arguments.size() - 5], delta_table);

    const Expression &equality_proof_uncasted =
        *arguments[arguments.size() - 5];
    if (typeid(equality_proof_uncasted) != typeid(ApplicationExpression))
      return;
    const ApplicationExpression &equality_proof =
        static_cast<const ApplicationExpression &>(equality_proof_uncasted);
    if (typeid(equality_proof.function()) != typeid(ReflexiveKeywordExpression))
      return;

    head = std::move(arguments[arguments.size() - 6]);
    for (int i = 0; i < 6; ++i) arguments.pop_back();
    visit(*head);
  }

  void visit_induction_keyword(InductionKeywordExpression &) override {
    if (arguments.size() < 4) return;
    arguments[arguments.size() - 4] =
        reduce(*arguments[arguments.size() - 4], delta_table);

    Expression &target_uncasted = *arguments[arguments.size() - 4];
    if (typeid(target_uncasted) == typeid(NatLiteralExpression)) {
      NatLiteralExpression &target_as_natural =
          static_cast<NatLiteralExpression &>(target_uncasted);

      if (target_as_natural.value == 0) {
        head = std::move(arguments[arguments.size() - 3]);
        for (int i = 0; i < 4; ++i) arguments.pop_back();
        visit(*head);
        return;
      }

      // Expand the target
      --target_as_natural.value;
      arguments[arguments.size() - 4] = std::make_unique<ApplicationExpression>(
          std::make_unique<SuccKeywordExpression>(),
          std::move(arguments[arguments.size() - 4]));
    }
    Expression &target_uncasted_new = *arguments[arguments.size() - 4];
    if (typeid(target_uncasted_new) == typeid(ApplicationExpression)) {
      ApplicationExpression &target =
          static_cast<ApplicationExpression &>(target_uncasted_new);
      if (typeid(target.function()) != typeid(SuccKeywordExpression)) return;

      std::unique_ptr<Expression> target_predecessor =
          std::move(target.argument());

      std::unique_ptr<Expression> expanded =
          std::make_unique<ApplicationExpression>(
              std::make_unique<ApplicationExpression>(
                  std::make_unique<ApplicationExpression>(
                      std::make_unique<ApplicationExpression>(
                          std::move(head),
                          std::move(arguments[arguments.size() - 1])),
                      arguments[arguments.size() - 2]->clone()),
                  std::move(arguments[arguments.size() - 3])),
              std::move(target_predecessor));

      head = std::move(arguments[arguments.size() - 2]);
      for (int i = 0; i < 4; ++i) arguments.pop_back();
      arguments.push_back(std::move(expanded));
      visit(*head);
    }
  }

  std::vector<std::unique_ptr<Expression>> arguments;
  std::unique_ptr<Expression> head;
  const std::unordered_map<std::pair<std::string, Expression *>, const Expression *,
                           IdHash> *delta_table;

 private:
  std::shared_ptr<spdlog::logger> get_logger() {
    static std::shared_ptr<spdlog::logger> logger =
        ([&]() -> std::shared_ptr<spdlog::logger> {
          logger = spdlog::stderr_color_mt("script3025::WHNFVisitor",
                                           spdlog::color_mode::always);
          logger->set_level(spdlog::level::warn);
          logger->set_pattern("%^[%l] [tid=%t] [%T.%F] [%s:%#] %v%$");
          return logger;
        })();
    return logger;
  }
};

void LazyReductionVisitor::visit_application(ApplicationExpression &e) {
  WHNFVisitor visitor;
  visitor.delta_table = delta_table;
  e.accept(visitor);
  std::vector<std::unique_ptr<Expression>> unapplied_arguments =
      std::move(visitor.arguments);

  std::unique_ptr<Expression> merged_expression =
      reduce(*visitor.head, delta_table);

  // Combine everything back into the head.
  while (!unapplied_arguments.empty()) {
    std::unique_ptr reduced_argument =
        reduce(*unapplied_arguments.back(), delta_table);
    unapplied_arguments.pop_back();
    merged_expression = std::make_unique<ApplicationExpression>(
        std::move(merged_expression), std::move(reduced_argument));
  }

  reduced_expression_ = std::move(merged_expression);
}

void LazyReductionVisitor::visit_id(IdExpression &e) {
  visit_expression(e);
  IdExpression &reduced_expression_casted =
      static_cast<IdExpression &>(*reduced_expression_);
  reduced_expression_casted.id = std::move(e.id);
  reduced_expression_casted.source = e.source;
}

void LazyReductionVisitor::visit_expression(Expression &e) {
  std::unique_ptr<Expression> reduced = make_default_like(e);

  for (size_t i = 0; i < e.children.size(); ++i) {
    reduced->children[i] = reduce(*e.children[i], delta_table);
  }

  reduced_expression_ = std::move(reduced);
}

std::shared_ptr<spdlog::logger> LazyReductionVisitor::get_logger() {
  static std::shared_ptr<spdlog::logger> logger =
      ([&]() -> std::shared_ptr<spdlog::logger> {
        logger = spdlog::stderr_color_mt("script3025::LazyReductionVisitor",
                                         spdlog::color_mode::always);
        logger->set_level(spdlog::level::warn);
        logger->set_pattern("%^[%l] [tid=%t] [%T.%F] [%s:%#] %v%$");
        return logger;
      })();
  return logger;
}

}  // namespace script3025
