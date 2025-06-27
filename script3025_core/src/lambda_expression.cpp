#include "lambda_expression.hpp"
#include "pi_expression.hpp"

namespace script3025 {

LambdaExpression::LambdaExpression(std::string &&argument_id,
                                   std::unique_ptr<Expression> &&argument_type,
                                   std::unique_ptr<Expression> &&definition)
                                   : argument_id(argument_id),
                                   argument_type(std::move(argument_type)),
                                   definition(std::move(definition)),
                                   Expression(
                                      argument_type -> parent_abstraction,
                                      std::unique_ptr<Expression>(), true) {
  this -> definition -> parent_abstraction = this;
}

LambdaExpression::LambdaExpression(const LambdaExpression &other)
    : argument_id(other.argument_id),
    argument_type(other.argument_type -> clone()),
    definition(other.definition -> clone()), Expression(other) {}

std::unique_ptr<Expression> LambdaExpression::clone() const {
  return std::make_unique<LambdaExpression>(*this);
}

bool LambdaExpression::is_normal() const {
  return argument_type -> is_normal() && definition -> is_normal();
}

Expression *LambdaExpression::get_type() {
  if (possibly_well_typed && !type){
    Expression *output_type = definition -> get_type();
    if (output_type == nullptr) {
      possibly_well_typed = false;
    } else {
      type = std::make_unique<PiExpression>(std::string(argument_id),
                                            argument_type -> clone(),
                                            output_type -> clone());
      type -> parent_abstraction = parent_abstraction;
      possibly_well_typed = true;
    }
  }

  if (type) {
    return type.get();
  } else {
    SPDLOG_LOGGER_ERROR(get_logger(), "Could not find type for expression {}", to_string());
    return nullptr;
  }
}

std::unique_ptr<Expression> LambdaExpression::replace(
    const std::string &id, const Expression *source,
    const Expression &expression) {
  std::unique_ptr<LambdaExpression> output =
      std::make_unique<LambdaExpression>(*this);

  output -> argument_type =
      output -> argument_type -> replace(id, source, expression);
  output -> definition =
      output -> definition -> replace(id, source, expression);

  return output;
}

std::unique_ptr<Expression> LambdaExpression::reduce() {
  std::unique_ptr<LambdaExpression> output =
      std::make_unique<LambdaExpression>(*this);
  output -> definition = output -> definition -> reduce();
  output -> argument_type = output -> argument_type -> reduce();
  return output;
};

bool LambdaExpression::operator==(const Expression &other) const {
  if(typeid(other) != typeid(LambdaExpression)) return false;
  const LambdaExpression &other_lambda =
      static_cast<const LambdaExpression &>(other);
  return argument_id == other_lambda.argument_id &&
         *argument_type == *other_lambda.argument_type &&
         *definition == *other_lambda.definition;
}

std::ostream &LambdaExpression::print(std::ostream &os) const {
  return os << "λ (" << argument_id << " : " << *argument_type << "). " <<
        *definition;
}

std::shared_ptr<spdlog::logger> LambdaExpression::get_logger() {
  static std::shared_ptr<spdlog::logger> logger =
      ([&] () -> std::shared_ptr<spdlog::logger> {
        logger = spdlog::stderr_color_mt("script3025::LambdaExpression", spdlog::color_mode::always);
        logger-> set_level(spdlog::level::warn);
        logger-> set_pattern("%^[%l] [tid=%t] [%T.%F] [%s:%#] %v%$");
        return logger;
      })();
  return logger;
}

void LambdaExpression::accept(ExpressionVisitor &visitor) const {
  visitor.visit_lambda(*this);
}

void LambdaExpression::accept(MutatingExpressionVisitor &visitor) {
  visitor.visit_lambda(*this);
}

}
