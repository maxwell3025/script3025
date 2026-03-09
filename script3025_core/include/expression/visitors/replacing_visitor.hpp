#ifndef SCRIPT3025_SCRIPT3025_CORE_REPLACING_VISITOR_HPP
#define SCRIPT3025_SCRIPT3025_CORE_REPLACING_VISITOR_HPP

#include <string>

#include "expression/expression_base.hpp"
#include "expression/expression_visitor.hpp"
#include "expression/visitors/cloning_visitor.hpp"

namespace script3025 {

class ReplacingVisitor : public CloningVisitor {
 public:
  // Construct a replacing visitor that replaces id expressions whose source pointer and id string match.
  // # Parameters
  // - `search_source`: the pointer that id expressions must match in order to be replaced.
  // - `search_id`: the id string that id expressions must match in order to be replaced.
  // - `replacement`: the expression that matching id expressions will be replaced with.
  ReplacingVisitor(Expression *search_source, std::string search_id,
                   Expression *replacement);
  void visit_id(const IdExpression &e) override;

 private:
  Expression *search_source_;
  std::string search_id_;
  Expression *replacement_;
};

}  // namespace script3025

#endif
