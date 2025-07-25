#ifndef SCRIPT3025_CORE_EXPRESSION_VISITORS_NORMALIZING_VISITOR_HPP
#define SCRIPT3025_CORE_EXPRESSION_VISITORS_NORMALIZING_VISITOR_HPP

#include <memory>

#include "expression/expression_visitor.hpp"
#include "spdlog/spdlog.h"

namespace script3025 {

class NormalizingVisitor : public MutatingExpressionVisitor {
 public:
  void visit_id(IdExpression &e) override;
  void visit_lambda(LambdaExpression &e) override;
  void visit_let(LetExpression &e) override;
  void visit_pi(PiExpression &e) override;

 protected:
  void visit_default(Expression &e);

 private:
  Expression *get_source(std::string name);
  void push_source(std::string name, Expression *source);
  void pop_source(std::string name);
  static std::shared_ptr<spdlog::logger> get_logger();

  std::unordered_map<std::string, std::vector<Expression *>> lexical_scope_;
};

}  // namespace script3025

#endif
