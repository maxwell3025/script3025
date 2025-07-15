#include "expression/visitors/replacing_visitor.hpp"

#include "expression/visitors/cloning_visitor.hpp"
#include "expression/subtypes/application_expression.hpp"
#include "expression/subtypes/id_expression.hpp"
#include "expression/subtypes/lambda_expression.hpp"
#include "expression/subtypes/let_expression.hpp"
#include "expression/subtypes/pi_expression.hpp"

namespace script3025 {

void ReplacingVisitor::visit_application(const ApplicationExpression &e) {
  std::unique_ptr<ApplicationExpression> new_expression =
      std::make_unique<ApplicationExpression>();
  pointer_map[&e] = new_expression.get();

  if (e.argument) {
    e.argument -> accept(*this);
    new_expression -> argument = std::move(value);
  }

  if (e.function) {
    e.function -> accept(*this);
    new_expression -> function = std::move(value);
  }

  value = std::move(new_expression);
}

void ReplacingVisitor::visit_id(const IdExpression &e) {
  if (e.source == target) {
    value = replacement -> clone();

    pointer_map[&e] = value.get();
  } else {
    std::unique_ptr<IdExpression> new_expression =
        std::make_unique<IdExpression>();

    pointer_map[&e] = new_expression.get();

    new_expression -> id = e.id;

    if (pointer_map.find(e.source) != pointer_map.end()) {
      new_expression -> source = pointer_map[e.source];
    } else {
      new_expression -> source = e.source;
    }

    value = std::move(new_expression);
  }
}

void ReplacingVisitor::visit_lambda(const LambdaExpression &e) {
  std::unique_ptr<LambdaExpression> new_expression =
      std::make_unique<LambdaExpression>();
  pointer_map[&e] = new_expression.get();

  new_expression -> argument_id = e.argument_id;

  if (e.argument_type) {
    e.argument_type -> accept(*this);
    new_expression -> argument_type = std::move(value);
  }

  if (e.definition) {
    e.definition -> accept(*this);
    new_expression -> definition = std::move(value);
  }

  value = std::move(new_expression);
}

void ReplacingVisitor::visit_let(const LetExpression &e) {
  std::unique_ptr<LetExpression> new_expression =
      std::make_unique<LetExpression>();
  pointer_map[&e] = new_expression.get();

  new_expression -> argument_id = e.argument_id;

  if (e.argument_value) {
    e.argument_value -> accept(*this);
    new_expression -> argument_value = std::move(value);
  }

  if (e.argument_type) {
    e.argument_type -> accept(*this);
    new_expression -> argument_type = std::move(value);
  }

  if (e.definition) {
    e.definition -> accept(*this);
    new_expression -> definition = std::move(value);
  }

  value = std::move(new_expression);
}

void ReplacingVisitor::visit_pi(const PiExpression &e) {
  std::unique_ptr<PiExpression> new_expression =
      std::make_unique<PiExpression>();
  pointer_map[&e] = new_expression.get();

  new_expression -> argument_id = e.argument_id;

  if (e.argument_type) {
    e.argument_type -> accept(*this);
    new_expression -> argument_type = std::move(value);
  }

  if (e.definition) {
    e.definition -> accept(*this);
    new_expression -> definition = std::move(value);
  }

  value = std::move(new_expression);
}

} // namespace script3025
