#ifndef SCRIPT3025_SCRIPT3025_CORE_REPLACING_VISITOR_HPP
#define SCRIPT3025_SCRIPT3025_CORE_REPLACING_VISITOR_HPP

#include "expression/expression_base.hpp"
#include "expression/expression_visitor.hpp"
#include "expression/visitors/cloning_visitor.hpp"

namespace script3025 {

class ReplacingVisitor : public CloningVisitor {
 public:
  ReplacingVisitor(Expression *search_source, std::string search_id,
                   Expression *replacement);
  void visit_id(const IdExpression &e) override;

 private:
  Expression *search_source;
  std::string search_id;
  Expression *replacement;
};

}  // namespace script3025

#endif
