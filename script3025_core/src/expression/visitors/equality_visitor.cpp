#include "expression/visitors/equality_visitor.hpp"

#include "expression/subtypes/application_expression.hpp"
#include "expression/subtypes/id_expression.hpp"
#include "expression/subtypes/lambda_expression.hpp"
#include "expression/subtypes/let_expression.hpp"
#include "expression/subtypes/pi_expression.hpp"

namespace script3025 {

EqualityVisitor::EqualityVisitor() : unequal(false) {}

void EqualityVisitor::visit_application(const ApplicationExpression &rhs) {
  pointer_map[lhs] = &rhs;

  if (typeid(lhs) != typeid(ApplicationExpression)) {
    unequal = true;
    return;
  }

  const ApplicationExpression *casted_lhs =
      static_cast<const ApplicationExpression *>(lhs);

  if (rhs.function) {
    lhs = casted_lhs -> function.get();
    rhs.function -> accept(*this);
  } else {
    unequal |= static_cast<bool>(casted_lhs -> function);
  }

  if (unequal) return;

  if (rhs.argument) {
    lhs = casted_lhs -> argument.get();
    rhs.argument -> accept(*this);
  } else {
    unequal |= static_cast<bool>(casted_lhs -> argument);
  }
}

void EqualityVisitor::visit_id(const IdExpression &rhs) {
  pointer_map[lhs] = &rhs;

  if (typeid(lhs) != typeid(IdExpression)) {
    unequal = true;
    return;
  }

  const IdExpression *casted_lhs = static_cast<const IdExpression *>(lhs);

  const Expression *translated_lhs_source = casted_lhs -> source;
  if (pointer_map.find(translated_lhs_source) != pointer_map.end())
      translated_lhs_source = pointer_map[translated_lhs_source];
  
  if(translated_lhs_source != rhs.source)
      unequal = true;
}

void EqualityVisitor::visit_lambda(const LambdaExpression &rhs) {
  pointer_map[lhs] = &rhs;

  if (typeid(lhs) != typeid(LambdaExpression)) {
    unequal = true;
    return;
  }

  const LambdaExpression *casted_lhs = static_cast<const LambdaExpression *>(lhs);

  if (rhs.argument_type) {
    lhs = casted_lhs -> argument_type.get();
    rhs.argument_type -> accept(*this);
  } else {
    unequal |= static_cast<bool>(casted_lhs -> argument_type);
  }

  if (unequal) return;

  if (rhs.definition) {
    lhs = casted_lhs -> definition.get();
    rhs.definition -> accept(*this);
  } else {
    unequal |= static_cast<bool>(casted_lhs -> definition);
  }
}

void EqualityVisitor::visit_let(const LetExpression &rhs) {
  pointer_map[lhs] = &rhs;

  if (typeid(lhs) != typeid(LetExpression)) {
    unequal = true;
    return;
  }

  const LetExpression *casted_lhs = static_cast<const LetExpression *>(lhs);

  if (rhs.argument_value) {
    lhs = casted_lhs -> argument_value.get();
    rhs.argument_value -> accept(*this);
  } else {
    unequal |= static_cast<bool>(casted_lhs -> argument_value);
  }

  if (unequal) return;

  if (rhs.argument_type) {
    lhs = casted_lhs -> argument_type.get();
    rhs.argument_type -> accept(*this);
  } else {
    unequal |= static_cast<bool>(casted_lhs -> argument_type);
  }

  if (unequal) return;

  if (rhs.definition) {
    lhs = casted_lhs -> definition.get();
    rhs.definition -> accept(*this);
  } else {
    unequal |= static_cast<bool>(casted_lhs -> definition);
  }
}

void EqualityVisitor::visit_pi(const PiExpression &rhs) {
  pointer_map[lhs] = &rhs;

  if (typeid(lhs) != typeid(PiExpression)) {
    unequal = true;
    return;
  }

  const PiExpression *casted_lhs = static_cast<const PiExpression *>(lhs);

  if (rhs.argument_type) {
    lhs = casted_lhs -> argument_type.get();
    rhs.argument_type -> accept(*this);
  } else {
    unequal |= static_cast<bool>(casted_lhs -> argument_type);
  }

  if (unequal) return;

  if (rhs.definition) {
    lhs = casted_lhs -> definition.get();
    rhs.definition -> accept(*this);
  } else {
    unequal |= static_cast<bool>(casted_lhs -> definition);
  }
}

} // namespace script3025