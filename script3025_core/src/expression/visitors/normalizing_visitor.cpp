#include "expression/visitors/normalizing_visitor.hpp"

#include "expression/expression.hpp"

namespace script3025 {

void NormalizingVisitor::visit_id(IdExpression &e) {
  e.source = get_source(e.id);
}

void NormalizingVisitor::visit_lambda(LambdaExpression &e) {
  visit(*e.argument_type());
  push_source(e.argument_id, &e);
  visit(*e.definition());
  pop_source(e.argument_id);
}

void NormalizingVisitor::visit_let(LetExpression &e) {
  visit(*e.argument_type());
  visit(*e.argument_value());
  push_source(e.argument_id, &e);
  visit(*e.definition());
  pop_source(e.argument_id);
}

void NormalizingVisitor::visit_pi(PiExpression &e) {
  visit(*e.argument_type());
  push_source(e.argument_id, &e);
  visit(*e.definition());
  pop_source(e.argument_id);
}

void NormalizingVisitor::visit_default(Expression &e) {
  for (std::unique_ptr<Expression> &child : e.children) {
    visit(*child);
  }
}

Expression *NormalizingVisitor::get_source(std::string name) {
  if (lexical_scope_[name].size() == 0) return nullptr;
  return lexical_scope_[name].back();
}

void NormalizingVisitor::push_source(std::string name, Expression *source) {
  lexical_scope_[name].push_back(source);
}

void NormalizingVisitor::pop_source(std::string name) {
  lexical_scope_[name].pop_back();
}

}  // namespace script3025