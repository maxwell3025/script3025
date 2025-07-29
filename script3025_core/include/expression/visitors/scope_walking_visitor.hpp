#ifndef SCRIPT3025_CORE_EXPRESSION_VISITORS_SCOPE_WALKING_VISITOR_HPP
#define SCRIPT3025_CORE_EXPRESSION_VISITORS_SCOPE_WALKING_VISITOR_HPP

#include <memory>

#include "expression/expression.hpp"
#include "expression/expression_visitor.hpp"
#include "spdlog/spdlog.h"

namespace script3025 {

template <const bool is_const>
class ScopeWalkingVisitor : public ExpressionVisitor<is_const> {
 public:
  template <typename T>
  using ref = std::conditional_t<is_const, const T &, T &>;

  using expression_ptr =
      std::conditional_t<is_const, const Expression *, Expression *>;

  void visit_lambda(ref<LambdaExpression> e) override {
    this->visit(*e.argument_type());
    push_source(e.argument_id, &e);
    this->visit(*e.definition());
    pop_source(e.argument_id);
  }

  void visit_let(ref<LetExpression> e) override {
    this->visit(*e.argument_type());
    this->visit(*e.argument_value());
    push_source(e.argument_id, &e);
    this->visit(*e.definition());
    pop_source(e.argument_id);
  }

  void visit_pi(ref<PiExpression> e) override {
    this->visit(*e.argument_type());
    push_source(e.argument_id, &e);
    this->visit(*e.definition());
    pop_source(e.argument_id);
  }

 protected:
  void visit_expression(ref<Expression> e) override {
    for (ref<std::unique_ptr<Expression>> child : e.children) {
      this->visit(*child);
    }
  }

  expression_ptr get_source(std::string name) {
    if (lexical_scope_[name].size() == 0) return nullptr;
    return lexical_scope_[name].back();
  }

  void push_source(std::string name, expression_ptr source) {
    lexical_scope_[name].push_back(source);
  }

  void pop_source(std::string name) { lexical_scope_[name].pop_back(); }

  std::unordered_map<std::string, std::vector<expression_ptr>> lexical_scope_;
};

}  // namespace script3025

#endif
