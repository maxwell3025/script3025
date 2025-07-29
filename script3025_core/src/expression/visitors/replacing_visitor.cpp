#include "expression/visitors/replacing_visitor.hpp"

#include "expression/expression.hpp"

namespace script3025 {

ReplacingVisitor::ReplacingVisitor(Expression *search_source,
                                   std::string search_id,
                                   Expression *replacement)
    : search_source(search_source),
      search_id(std::move(search_id)),
      replacement(replacement) {}

void ReplacingVisitor::visit_id(const IdExpression &e) {
  if (e.source == search_source && e.id == search_id) {
    CloningVisitor visitor;
    visitor.visit(*replacement);
    value_ = visitor.get();
    pointer_map_[&e] = value_.get();
  } else {
    CloningVisitor::visit_id(e);
  }
}

}  // namespace script3025
