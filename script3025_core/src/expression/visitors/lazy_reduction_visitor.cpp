#include "expression/visitors/lazy_reduction_visitor.hpp"

#include "expression/visitors/replacing_visitor.hpp"
#include "expression/subtypes/application_expression.hpp"
#include "expression/subtypes/id_expression.hpp"
#include "expression/subtypes/lambda_expression.hpp"
#include "expression/subtypes/let_expression.hpp"
#include "expression/subtypes/pi_expression.hpp"

namespace script3025 {

class WHNFVisitor : public MutatingExpressionVisitor {
 public:
  void visit_application(ApplicationExpression &e) {
    arguments.push_back(std::move(e.argument));
    head = std::move(e.function);
    head -> accept(*this);
  }

  void visit_id(IdExpression &e) {
    // TODO condition on reduction type
  
  }
  
  void visit_lambda(LambdaExpression &e) {
    // TODO condition on reduction type
    ReplacingVisitor visitor;
    visitor.target = &e;
    visitor.replacement = arguments.back().get();
    e.definition -> accept(visitor);
    head = std::move(visitor.value);
    arguments.pop_back();
  }
  
  void visit_let(LetExpression &e) {
    // TODO condition on reduction type
    ReplacingVisitor visitor;
    visitor.target = &e;
    visitor.replacement = e.argument_value.get();
    e.definition -> accept(visitor);
    head = std::move(visitor.value);
    arguments.pop_back();
  }
  
  void visit_pi(PiExpression &e) {
  
  }

  std::vector<std::unique_ptr<Expression>> arguments;
  std::unique_ptr<Expression> head;
};

void LazyReductionVisitor::visit_application(ApplicationExpression &e) {
  WHNFVisitor visitor;
  e.accept(visitor);
  std::vector<std::unique_ptr<Expression>> arguments = std::move(visitor.arguments);
  std::unique_ptr<Expression> head = std::move(visitor.head);

  head -> accept(*this);
  head = std::move(reduced_expression);

  while(!arguments.empty()) {
    std::unique_ptr<ApplicationExpression> new_head =
        std::make_unique<ApplicationExpression>();

    arguments.back() -> accept(*this);
    new_head -> argument = std::move(reduced_expression);

    new_head -> function = std::move(head);

    head = std::move(new_head);
  }

  reduced_expression = std::move(head);
}

void LazyReductionVisitor::visit_id(IdExpression &e) {}

void LazyReductionVisitor::visit_lambda(LambdaExpression &e) {
  std::unique_ptr<LambdaExpression> reduced_lambda =
      std::make_unique<LambdaExpression>();

  e.argument_type -> accept(*this);
  reduced_lambda -> argument_type = std::move(reduced_expression);

  e.definition -> accept(*this);
  reduced_lambda -> definition = std::move(reduced_expression);

  reduced_expression = std::move(reduced_lambda);
}

void LazyReductionVisitor::visit_let(LetExpression &e) {
  std::unique_ptr<LetExpression> reduced_let =
      std::make_unique<LetExpression>();

  e.argument_value -> accept(*this);
  reduced_let -> argument_value = std::move(reduced_expression);

  e.argument_type -> accept(*this);
  reduced_let -> argument_type = std::move(reduced_expression);

  e.definition -> accept(*this);
  reduced_let -> definition = std::move(reduced_expression);

  reduced_expression = std::move(reduced_let);
}

void LazyReductionVisitor::visit_pi(PiExpression &e) {
  std::unique_ptr<PiExpression> reduced_pi =
      std::make_unique<PiExpression>();

  e.argument_type -> accept(*this);
  reduced_pi -> argument_type = std::move(reduced_expression);

  e.definition -> accept(*this);
  reduced_pi -> definition = std::move(reduced_expression);

  reduced_expression = std::move(reduced_pi);
}

} // namespace script3025
