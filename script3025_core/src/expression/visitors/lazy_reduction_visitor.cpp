#include "expression/visitors/lazy_reduction_visitor.hpp"

#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <cstddef>
#include <iostream>
#include <memory>
#include <sstream>
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
#include "expression/variable_reference.hpp"
#include "expression/visitors/replacing_visitor.hpp"

namespace script3025 {

namespace {

std::unique_ptr<Expression> make_application_chain(
    std::unique_ptr<Expression> head) {
  return head;
}

// Helper function to construct a left-associative application chain from a
// vector of arguments.
template <typename... Tail>
std::unique_ptr<Expression> make_application_chain(
    std::unique_ptr<Expression> func, std::unique_ptr<Expression> arg,
    Tail... tail) {
  return make_application_chain(
      std::make_unique<ApplicationExpression>(std::move(func), std::move(arg)),
      std::move(tail)...);
}

}  // namespace

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
          "{}\n",
          head->to_string(), arguments.back()->to_string());
    }
  }

  void visit_id(IdExpression &e) override {
    auto replacement_it = delta_table->find(VariableReference{e.id, e.source});
    if (replacement_it != delta_table->end()) {
      const Expression &replacement = *(replacement_it->second);
      head = replacement.clone();
      visit(*head);
    }
  }

  void visit_lambda(LambdaExpression &e) override {
    if (arguments.size() < 1) return;
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
          "{}\n",
          head->to_string(), arguments.back()->to_string());
    }
  }

  void visit_replace_keyword(ReplaceKeywordExpression &) override {
    // Subject reduction is only conserved if the equality proof is a direct
    // reflexive proof.
    // Another perspective is that this prevents substitution using equality
    // proofs constructed by inducting through opaque symbols.
    if (arguments.size() < 5) return;

    std::unique_ptr<Expression> parameter_ptr = std::move(arguments.back());
    arguments.pop_back();
    std::unique_ptr<Expression> motive_ptr = std::move(arguments.back());
    arguments.pop_back();
    std::unique_ptr<Expression> minor_premise_ptr = std::move(arguments.back());
    arguments.pop_back();
    std::unique_ptr<Expression> major_premise_index_ptr =
        std::move(arguments.back());
    arguments.pop_back();
    std::unique_ptr<Expression> major_premise_ptr = std::move(arguments.back());
    arguments.pop_back();

    reduce_inplace(major_premise_ptr, *delta_table);

    // Clang warns here to tell the user that typeid is a runtime function and
    // not a compile time macro, as many would expect.
    // I already know this.
    // NOLINTNEXTLINE
    if (typeid(*major_premise_ptr) != typeid(ApplicationExpression)) {
      arguments.emplace_back(std::move(major_premise_ptr));
      arguments.emplace_back(std::move(major_premise_index_ptr));
      arguments.emplace_back(std::move(minor_premise_ptr));
      arguments.emplace_back(std::move(motive_ptr));
      arguments.emplace_back(std::move(parameter_ptr));
      return;
    }

    const ApplicationExpression &major_premise =
        static_cast<const ApplicationExpression &>(*major_premise_ptr);

    if (typeid(major_premise.function()) !=
        typeid(ReflexiveKeywordExpression)) {
      arguments.emplace_back(std::move(major_premise_ptr));
      arguments.emplace_back(std::move(major_premise_index_ptr));
      arguments.emplace_back(std::move(minor_premise_ptr));
      arguments.emplace_back(std::move(motive_ptr));
      arguments.emplace_back(std::move(parameter_ptr));
      return;
    }

    head = std::move(minor_premise_ptr);
    visit(*head);
  }

  void visit_induction_keyword(InductionKeywordExpression &) override {
    if (arguments.size() < 4) return;
    SPDLOG_LOGGER_TRACE(get_logger(),
                        "Attempting to reduce induction expression.\n"
                        "Current head:\n"
                        "  {}\n"
                        "Current arguments:\n"
                        "{}",
                        head->to_string(), [&]() {
                          std::stringstream ss;
                          for (const auto &argument : arguments) {
                            ss << "  " << argument->to_string() << std::endl;
                          }
                          return ss.str();
                        }());

    std::unique_ptr<Expression> motive_ptr = std::move(arguments.back());
    arguments.pop_back();
    std::unique_ptr<Expression> minor_premise_1_ptr =
        std::move(arguments.back());
    arguments.pop_back();
    std::unique_ptr<Expression> minor_premise_2_ptr =
        std::move(arguments.back());
    arguments.pop_back();
    std::unique_ptr<Expression> major_premise_ptr = std::move(arguments.back());
    arguments.pop_back();

    reduce_inplace(major_premise_ptr, *delta_table);

    // Here, we Expand the major premise into a succ n expression.
    // Clang warns here to tell the user that typeid is a runtime function and
    // not a compile time macro, as many would expect.
    // I already know this.
    // NOLINTNEXTLINE
    if (typeid(*major_premise_ptr) == typeid(NatLiteralExpression)) {
      NatLiteralExpression &major_premise =
          static_cast<NatLiteralExpression &>(*major_premise_ptr);

      if (major_premise.value == 0) {
        head = std::move(minor_premise_2_ptr);
        visit(*head);
        return;
      }

      // Expand the target
      --major_premise.value;
      major_premise_ptr = std::make_unique<ApplicationExpression>(
          std::make_unique<SuccKeywordExpression>(),
          std::move(major_premise_ptr));
    }

    // Clang warns here to tell the user that typeid is a runtime function and
    // not a compile time macro, as many would expect.
    // I already know this.
    // NOLINTNEXTLINE
    if (typeid(*major_premise_ptr) == typeid(ApplicationExpression)) {
      ApplicationExpression &major_premise =
          static_cast<ApplicationExpression &>(*major_premise_ptr);

      reduce_inplace(major_premise.function(), *delta_table);

      // Clang warns here to tell the user that typeid is a runtime function and
      // not a compile time macro, as many would expect.
      // I already know this.
      // NOLINTNEXTLINE
      if (typeid(*major_premise.function()) != typeid(SuccKeywordExpression)) {
        arguments.emplace_back(std::move(major_premise_ptr));
        arguments.emplace_back(std::move(minor_premise_2_ptr));
        arguments.emplace_back(std::move(minor_premise_1_ptr));
        arguments.emplace_back(std::move(motive_ptr));
        return;
      }

      std::unique_ptr<Expression> minor_premise_1_applied =
          make_application_chain(minor_premise_1_ptr->clone(),
                                 major_premise.argument()->clone());

      std::unique_ptr<Expression> subgoal = make_application_chain(
          std::move(head), std::move(motive_ptr),
          std::move(minor_premise_1_ptr), std::move(minor_premise_2_ptr),
          std::move(major_premise.argument()));

      head = std::move(minor_premise_1_applied);
      arguments.push_back(std::move(subgoal));
      SPDLOG_LOGGER_TRACE(
          get_logger(),
          "Evaluated induction expression with minor premise 1.\n"
          "Current head:\n"
          "  {}\n"
          "Current arguments:\n"
          "{}",
          head->to_string(), [&]() {
            std::stringstream ss;
            for (const auto &argument : arguments) {
              ss << "  " << argument->to_string() << std::endl;
            }
            return ss.str();
          }());
      visit(*head);
    }
  }

  void visit_succ_keyword(SuccKeywordExpression &) override {
    if (arguments.size() < 1) return;
    reduce_inplace(arguments[arguments.size() - 1], *delta_table);

    Expression &target_uncasted = *arguments[arguments.size() - 1];
    if (typeid(target_uncasted) == typeid(NatLiteralExpression)) {
      NatLiteralExpression &target_as_natural =
          static_cast<NatLiteralExpression &>(target_uncasted);

      ++target_as_natural.value;

      head = std::move(arguments.back());
      arguments.pop_back();
    }
  }

  std::vector<std::unique_ptr<Expression>> arguments;
  std::unique_ptr<Expression> head;
  const std::unordered_map<VariableReference, const Expression *> *delta_table;

 private:
  std::shared_ptr<spdlog::logger> get_logger() {
    static std::shared_ptr<spdlog::logger> logger =
        ([&]() -> std::shared_ptr<spdlog::logger> {
          logger = spdlog::stderr_color_mt("script3025::WHNFVisitor",
                                           spdlog::color_mode::always);
          logger->set_level(spdlog::level::trace);
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

  std::unique_ptr<Expression> merged_expression = std::move(visitor.head);
  visit(*merged_expression);
  merged_expression = get();

  // Combine everything back into the head.
  while (!unapplied_arguments.empty()) {
    std::unique_ptr<Expression> argument =
        std::move(unapplied_arguments.back());
    unapplied_arguments.pop_back();
    visit(*argument);
    merged_expression = std::make_unique<ApplicationExpression>(
        std::move(merged_expression), get());
  }

  set(std::move(merged_expression));
}

void LazyReductionVisitor::visit_id(IdExpression &e) {
  visit_expression(e);
  IdExpression &reduced_expression_casted =
      static_cast<IdExpression &>(get_ref());
  reduced_expression_casted.id = std::move(e.id);
  reduced_expression_casted.source = e.source;
}

void LazyReductionVisitor::visit_nat_literal(NatLiteralExpression &e) {
  visit_expression(e);
  NatLiteralExpression &reduced_expression_casted =
      static_cast<NatLiteralExpression &>(get_ref());
  reduced_expression_casted.value = e.value;
}

void LazyReductionVisitor::visit_expression(Expression &e) {
  // This uses clone instead of make_default_like to preserve non-default
  // fields.
  std::unique_ptr<Expression> reduced = e.clone();

  for (size_t i = 0; i < e.children.size(); ++i) {
    reduced->children[i] =
        reduce_unique_ptr(std::move(reduced->children[i]), *delta_table);
  }

  set(std::move(reduced));
}

std::shared_ptr<spdlog::logger> LazyReductionVisitor::get_logger() {
  static std::shared_ptr<spdlog::logger> logger =
      ([&]() -> std::shared_ptr<spdlog::logger> {
        logger = spdlog::stderr_color_mt("script3025::LazyReductionVisitor",
                                         spdlog::color_mode::always);
        logger->set_level(spdlog::level::trace);
        logger->set_pattern("%^[%l] [tid=%t] [%T.%F] [%s:%#] %v%$");
        return logger;
      })();
  return logger;
}

}  // namespace script3025
