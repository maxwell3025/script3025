#ifndef SCRIPT3025_CORE_EXPRESSION_VISITORS_SCOPE_WALKING_VISITOR_HPP
#define SCRIPT3025_CORE_EXPRESSION_VISITORS_SCOPE_WALKING_VISITOR_HPP

#include <memory>

#include "expression/expression.hpp"
#include "expression/expression_visitor.hpp"
#include "spdlog/spdlog.h"

namespace script3025 {

class ScopeWalkingVisitor : public MutatingExpressionVisitor {
 public:
  void visit_lambda(LambdaExpression &e) override {
    visit(*e.argument_type());
    push_source(e.argument_id, &e);
    visit(*e.definition());
    pop_source(e.argument_id);
  }

  void visit_let(LetExpression &e) override {
    visit(*e.argument_type());
    visit(*e.argument_value());
    push_source(e.argument_id, &e);
    visit(*e.definition());
    pop_source(e.argument_id);
  }

  void visit_pi(PiExpression &e) override {
    visit(*e.argument_type());
    push_source(e.argument_id, &e);
    visit(*e.definition());
    pop_source(e.argument_id);
  }

 protected:
  void visit_default(Expression &e) override {
    for (std::unique_ptr<Expression> &child : e.children) {
      visit(*child);
    }
  }

  Expression *get_source(std::string name) {
    if (lexical_scope_[name].size() == 0) return nullptr;
    return lexical_scope_[name].back();
  }

  void push_source(std::string name, Expression *source) {
    lexical_scope_[name].push_back(source);
  }

  void pop_source(std::string name) { lexical_scope_[name].pop_back(); }

  std::unordered_map<std::string, std::vector<Expression *>> lexical_scope_;
};

}  // namespace script3025

#endif
