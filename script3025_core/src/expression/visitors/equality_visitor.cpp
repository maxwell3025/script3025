#include "expression/visitors/equality_visitor.hpp"

#include <cstddef>
#include <sstream>

#include "expression/expression.hpp"
#include "spdlog/fmt/bundled/format.h"
#include "spdlog/spdlog.h"

namespace script3025 {

EqualityVisitor::EqualityVisitor(const Expression *lhs)
    : unequal_(false), lhs_(lhs) {}

void EqualityVisitor::visit_id(const IdExpression &rhs) {
  pointer_map_[lhs_] = &rhs;

  if (typeid(*lhs_) != typeid(IdExpression)) {
    unequal_ = true;
    SPDLOG_LOGGER_TRACE(get_logger(),
                        "Found expressions to be unequal by type.\n"
                        "    lhs: {}\n"
                        "    type of lhs: {}\n"
                        "    rhs: {}\n"
                        "    type of rhs: {}",
                        lhs_->to_string(), typeid(*lhs_).name(),
                        rhs.to_string(), typeid(rhs).name());
    return;
  }

  const IdExpression *casted_lhs = static_cast<const IdExpression *>(lhs_);

  const Expression *translated_lhs_source = casted_lhs->source;
  if (pointer_map_.find(translated_lhs_source) != pointer_map_.end())
    translated_lhs_source = pointer_map_[translated_lhs_source];

  if (translated_lhs_source != rhs.source) unequal_ = true;
  if (unequal_) {
    SPDLOG_LOGGER_TRACE(get_logger(),
                        "Found expressions to be unequal by Id source.\n"
                        "    lhs: {}\n"
                        "    lhs source: {}\n"
                        "    rhs: {}\n"
                        "    rhs source: {}",
                        lhs_->to_string(), fmt::ptr(translated_lhs_source),
                        rhs.to_string(), fmt::ptr(rhs.source));

    SPDLOG_LOGGER_TRACE(get_logger(), "Current pointer map:\n{}", [&]() {
      std::stringstream ss;
      for (const auto &[key, value] : pointer_map_) {
        ss << fmt::ptr(key) << " -> " << fmt::ptr(value) << "\n";
      }
      return ss.str();
    }());
  }
}

void EqualityVisitor::visit_nat_literal(const NatLiteralExpression &rhs) {
  visit_expression(rhs);
  if (static_cast<const NatLiteralExpression *>(lhs_)->value != rhs.value)
    unequal_ = true;
}

void EqualityVisitor::visit_type_keyword(const TypeKeywordExpression &rhs) {
  visit_expression(rhs);
  if (static_cast<const TypeKeywordExpression *>(lhs_)->level != rhs.level)
    unequal_ = true;
}

void EqualityVisitor::visit_expression(const Expression &rhs) {
  pointer_map_[lhs_] = &rhs;

  if (typeid(*lhs_) != typeid(rhs)) {
    SPDLOG_LOGGER_TRACE(get_logger(),
                        "Found expressions to be unequal by type.\n"
                        "    lhs: {}\n"
                        "    type of lhs: {}\n"
                        "    rhs: {}\n"
                        "    type of rhs: {}",
                        lhs_->to_string(), typeid(*lhs_).name(),
                        rhs.to_string(), typeid(rhs).name());
    unequal_ = true;
    return;
  }

  const Expression *lhs_stored = lhs_;
  for (size_t i = 0; i < rhs.children.size(); ++i) {
    if (rhs.children[i]) {
      lhs_ = lhs_stored->children[i].get();
      visit(*rhs.children[i]);
      if (unequal_) {
        SPDLOG_LOGGER_TRACE(
            get_logger(),
            "Found expressions to be unequal by child (bubbling up).\n"
            "    lhs expression: {}\n"
            "    rhs expression: {}\n"
            "    index: {}",
            lhs_stored->to_string(), rhs.to_string(), i);
      }
    } else {
      unequal_ |= static_cast<bool>(lhs_->children[i]);
      if (unequal_) {
        SPDLOG_LOGGER_TRACE(get_logger(),
                            "Found expressions to be unequal by null child.\n"
                            "    lhs expression: {}\n"
                            "    rhs expression: {}\n"
                            "    index: {}",
                            lhs_->to_string(), rhs.to_string(), i);
        return;
      }
    }
    if (unequal_) return;
  }
}

bool EqualityVisitor::get() const { return !unequal_; }

}  // namespace script3025