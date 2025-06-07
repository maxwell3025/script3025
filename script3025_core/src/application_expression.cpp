#include "expression.hpp"

namespace script3025 {

ApplicationExpression::ApplicationExpression(
    std::unique_ptr<Expression> &&function,
    std::unique_ptr<Expression> &&argument)
    : function(std::move(function)),
    argument(std::move(argument)) {}

ApplicationExpression::ApplicationExpression(const ApplicationExpression &other)
    : function(other.function -> clone()),
    argument(other.argument -> clone())
    {}

std::unique_ptr<Expression> ApplicationExpression::clone() const {
  return std::make_unique<ApplicationExpression>(*this);
}

bool ApplicationExpression::is_normal() const {
  return typeid(*function) == typeid(IdExpression);
}

Expression* ApplicationExpression::get_type() {
  if (possibly_well_typed && !type){
    Expression *function_type = function -> get_type();
    Expression *argument_type = argument -> get_type();
    if (function_type &&
        argument_type &&
        typeid(*function_type) == typeid(PiExpression)) {
      PiExpression &function_type_ref =
          static_cast<PiExpression&>(*function_type);
      type = function_type_ref.definition -> clone();
      type = type -> replace(function_type_ref.argument_id,
                             &function_type_ref,
                             *argument_type);
      type -> parent_abstraction = parent_abstraction;
      possibly_well_typed = true;
    } else {
      possibly_well_typed = false;
    }
  }

  if (type) {
    return type.get();
  } else {
    LOGGER3025_ERROR("Could not find type for expression {}", to_string(*this));
    return nullptr;
  }
}

std::unique_ptr<Expression> ApplicationExpression::replace(
    const std::string &id, const Expression *source,
    const Expression &expression) {
  std::unique_ptr<ApplicationExpression> output =
      std::make_unique<ApplicationExpression>(*this);

  output -> function = output -> function -> replace(id, source, expression);
  output -> argument = output -> argument -> replace(id, source, expression);

  return output;
}

std::unique_ptr<Expression> ApplicationExpression::get_whnf() {
  // This is our rolling value as we modify our current value.
  std::unique_ptr<Expression> output = clone();
  ApplicationExpression &output_as_app =
      static_cast<ApplicationExpression &>(*output);

  // TODO handle delta reductions

  // If our function subexpression is an application, WHNFify that first.
  if (typeid(*function) == typeid(ApplicationExpression))
    output_as_app.function =
        static_cast<ApplicationExpression &>(*function).get_whnf();

  // If we have a lambda literal on the left, then we should beta reduce.
  if (typeid(*output_as_app.function) == typeid(LambdaExpression)) {
    LambdaExpression &function_ref =
        static_cast<LambdaExpression &>(*output_as_app.function);

    // Sound alarms if a type error occurs
    if(*function_ref.type -> reduce() != *argument -> get_type())
      LOGGER3025_ERROR("Bad Application in {}", to_string(*this));

    // Actually do the beta reduction.
    // To implement cbv, use argument -> reduce() instead of argument.
    function_ref.definition =
        function_ref.definition -> replace(function_ref.argument_id,
                                           &function_ref,
                                           *argument);
    // This could potentially cause some weird move semantics bs depending on
    // how destructors are called.
    // i.e. since output owns function(_ref) owns definition, the assignment
    // operator might first destoroy output, which voids
    // function_ref.definition, which then gets assigned to output as
    // unique_ptr(nullptr).
    output = std::move(function_ref.definition);
  }

  return output;
}

std::unique_ptr<Expression> ApplicationExpression::reduce() {
  // This follows the lazy tactic in coq
  // see https://rocq-prover.org/doc/V8.18.0/refman/proofs/writing-proofs/equality.html#coq:tacn.lazy
  std::unique_ptr<Expression> output = clone();

  output = static_cast<ApplicationExpression&>(*output).get_whnf();

  // Now that we have our expression in WHNF, it is in the form F a b c ...,
  // where F, a, b, and c are possibly reducible subexpressions, and where F is
  // not top-level reducible.
  // Here, we recursively reduce all of F, a, b, and c by traversing down the
  // AST
  Expression *current_expression = output.get();
  ApplicationExpression *current_expression_as_application;
  while (typeid(*current_expression) == typeid(ApplicationExpression)) {
    current_expression_as_application =
        static_cast<ApplicationExpression*>(current_expression);
    current_expression_as_application -> argument =
        current_expression_as_application -> argument -> reduce();
    current_expression = current_expression_as_application -> function.get();
  }

  // Here, we reduce the leftmost lambda.
  // Currently, this doesn't do anything, since only lambdas, applications, and
  // Pis contain subexpressions, and none of those can be here.
  // However, once we implement match, this will become useful.
  current_expression_as_application -> function =
      current_expression_as_application -> function -> reduce();

  return output;
}

bool ApplicationExpression::operator==(const Expression &other) const {
  if(typeid(other) != typeid(ApplicationExpression)) return false;
  const ApplicationExpression &other_lambda =
      static_cast<const ApplicationExpression &>(other);
  return *function == *other_lambda.function &&
         *argument == *other_lambda.argument;
}

std::ostream &ApplicationExpression::print(std::ostream &os) const {
  return os << "(" << *function << ")(" << *argument << ")";
}

}
