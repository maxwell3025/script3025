#ifndef SCRIPT3025_SCRIPT3025_CORE_REFERENCE_STACK_HPP
#define SCRIPT3025_SCRIPT3025_CORE_REFERENCE_STACK_HPP

#include <memory>
#include <stdexcept>
#include <variant>
#include <vector>
#include <utility>

#include "expression/expression.hpp"
#include "expression/variable_reference.hpp"
#include "spdlog/sinks/stdout_color_sinks-inl.h"
#include "spdlog/spdlog.h"
#include "spdlog/common.h"

namespace script3025 {

// @brief An effcient stack of shadowable references.
class ReferenceStack {
 public:
  explicit ReferenceStack() = default;

  void push(const VariableReference ref, const Expression *value) {
    if (!write_lock_.unique()) {
      // SPDLOG_LOGGER_ERROR(get_logger(),
      //                     "Attempted to push to a reference stack that has "
      //                     "been extended. This likely means that the "
      //                     "reference stack is being used after it has been "
      //                     "extended, which is not allowed.\n"
      //                     "Reference: {}\n"
      //                     "Value: {}\n",
      //                     ref, value);
      return;
      throw std::runtime_error(
          "Attempted to push to a reference stack that has been extended.");
    }
    stack_.emplace_back(ref, value);
  }

  void pop(const VariableReference ref, const Expression *value) {
    if (!write_lock_.unique()) {
      // SPDLOG_LOGGER_ERROR(get_logger(),
      //                     "Attempted to pop from a reference stack that has "
      //                     "been extended. This likely means that the "
      //                     "reference stack is being used after it has been "
      //                     "extended, which is not allowed.\n"
      //                     "Reference: {}\n"
      //                     "Value: {}\n",
      //                     ref, value);
      throw std::runtime_error(
          "Attempted to pop from a reference stack that has been extended.");
    }
    if (stack_.empty()) {
      SPDLOG_LOGGER_ERROR(get_logger(),
                          "Attempted to pop from an empty reference stack.");
      throw std::runtime_error("Attempted to pop from an empty stack.");
    }
    if (stack_.back().first != ref || stack_.back().second != value) {
      // SPDLOG_LOGGER_ERROR(
      //     get_logger(),
      //     "Attempted to pop a reference that does not match the "
      //     "top of the stack.\n"
      //     "Expected reference: {}\n"
      //     "Actual reference: {}\n"
      //     "Expected value: {}\n"
      //     "Actual value: {}\n",
      //     stack_.back().first, ref, stack_.back().second, value);
      throw std::runtime_error("Invalid pop.");
    }
    stack_.pop_back();
  }

  [[nodiscard]] ReferenceStack extend() const {
    ReferenceStack new_stack;
    new_stack.parent_stack_ = this;
    new_stack.parent_lease_ = write_lock_;
    return new_stack;
  }

  [[nodiscard]] const Expression *lookup(const VariableReference ref) const {
    for (auto it = stack_.rbegin(); it != stack_.rend(); ++it) {
      if (it->first == ref) return it->second;
    }
    if (parent_stack_ != nullptr) {
      return parent_stack_->lookup(ref);
    }
    return nullptr;
  }

 private:
  [[nodiscard]] static std::shared_ptr<spdlog::logger> get_logger() {
    static std::shared_ptr<spdlog::logger> logger =
        ([&]() -> std::shared_ptr<spdlog::logger> {
          logger = spdlog::stderr_color_mt("script3025::ReferenceStack",
                                           spdlog::color_mode::always);
          logger->set_level(spdlog::level::trace);
          logger->set_pattern("%^[%l] [tid=%t] [%T.%F] [%s:%#] %v%$");
          return logger;
        })();
    return logger;
  }
  std::vector<std::pair<VariableReference, const Expression *>> stack_;
  std::shared_ptr<std::monostate> write_lock_ =
      std::make_shared<std::monostate>();
  std::shared_ptr<std::monostate> parent_lease_ = nullptr;
  const ReferenceStack *parent_stack_ = nullptr;
};

}  // namespace script3025

#endif