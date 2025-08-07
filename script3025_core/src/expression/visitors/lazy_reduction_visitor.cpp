#include "expression/visitors/lazy_reduction_visitor.hpp"

#include "expression/expression.hpp"
#include "expression/visitors/cloning_visitor.hpp"
#include "expression/visitors/replacing_visitor.hpp"
#include "partial_clone_visitor.hpp"

namespace script3025 {

struct IdHash {
  size_t operator()(std::pair<std::string, Expression *> id) const {
    return ((std::hash<std::string>{}(id.first)) << 1) ^
           ((std::hash<Expression *>{}(id.second)));
  }
};

class WHNFVisitor : public MutatingExpressionVisitor {
 public:
  void visit_application(ApplicationExpression &e) {
    arguments.push_back(std::move(e.argument()));
    head = std::move(e.function());
    visit(*head);
  }

  void visit_equality(PiExpression &) {
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

  void visit_id(IdExpression &e) {
    auto replacement_it = delta_table.find(std::make_pair(e.id, e.source));
    if (replacement_it != delta_table.end()) {
      Expression &replacement = *(replacement_it->second);
      CloningVisitor visitor;
      visitor.visit(replacement);
      head = visitor.get();
      visit(*head);
    }
  }

  void visit_lambda(LambdaExpression &e) {
    ReplacingVisitor visitor(&e, e.argument_id, arguments.back().get());
    visitor.visit(*e.definition());
    arguments.pop_back();
    head = std::move(visitor.get());
    visit(*head);
  }

  void visit_let(LetExpression &e) {
    ReplacingVisitor visitor(&e, e.argument_id, e.argument_value().get());
    visitor.visit(*e.definition());
    head = std::move(visitor.get());
    visit(*head);
  }

  void visit_pi(PiExpression &) {
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

  std::vector<std::unique_ptr<Expression>> arguments;
  std::unique_ptr<Expression> head;
  std::unordered_map<std::pair<std::string, Expression *>, Expression *, IdHash>
      delta_table;

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

// TODO none of this is pointer-correct. this needs to be fixed
void LazyReductionVisitor::visit_application(ApplicationExpression &e) {
  WHNFVisitor visitor;
  e.accept(visitor);
  std::vector<std::unique_ptr<Expression>> orphan_arguments =
      std::move(visitor.arguments);

  visit(*visitor.head);
  std::unique_ptr<Expression> merged_expression = std::move(reduced_expression);

  // Combine everything back into the head.
  while (!orphan_arguments.empty()) {
    std::unique_ptr<ApplicationExpression> new_head =
        std::make_unique<ApplicationExpression>();

    new_head->function() = std::move(merged_expression);

    visit(*orphan_arguments.back());
    new_head->argument() = std::move(reduced_expression);

    merged_expression = std::move(new_head);
  }

  reduced_expression = std::move(merged_expression);
}

void LazyReductionVisitor::visit_id(IdExpression &e) {
  visit_expression(e);
  IdExpression &reduced_expression_casted =
      static_cast<IdExpression &>(*reduced_expression);
  reduced_expression_casted.id = std::move(e.id);
  reduced_expression_casted.source = e.source;
}

void LazyReductionVisitor::visit_expression(Expression &e) {
  std::unique_ptr<Expression> reduced = make_default_like(e);

  for (size_t i = 0; i < e.children.size(); ++i) {
    visit(*e.children[i]);
    reduced->children[i] = std::move(reduced_expression);
  }

  reduced_expression = std::move(reduced);
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
