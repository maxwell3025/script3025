#include "expression/visitors/replacing_visitor.hpp"

#include <string>
#include <utility>

#include "expression/expression.hpp"
#include "expression/visitors/cloning_visitor.hpp"

namespace script3025 {

ReplacingVisitor::ReplacingVisitor(Expression *search_source,
                                   std::string search_id,
                                   Expression *replacement)
    : search_source_(search_source),
      search_id_(std::move(search_id)),
      replacement_(replacement) {}

void ReplacingVisitor::visit_id(const IdExpression &e) {
  if (e.source == search_source_ && e.id == search_id_) {
    CloningVisitor visitor;
    visitor.visit(*replacement_);
    value_ = visitor.get();
  } else {
    CloningVisitor::visit_id(e);
  }
}

}  // namespace script3025
