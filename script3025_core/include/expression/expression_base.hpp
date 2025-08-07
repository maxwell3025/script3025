#ifndef SCRIPT3025_SCRIPT3025_CORE_EXPRESSION_BASE_HPP
#define SCRIPT3025_SCRIPT3025_CORE_EXPRESSION_BASE_HPP

#include <string>

#include "parser.hpp"
#include "spdlog/fmt/fmt.h"
#include "token.hpp"

namespace script3025 {

template <const bool is_const>
class ExpressionVisitor;

class Expression {
 public:
  virtual void accept(ExpressionVisitor<true> &visitor) const = 0;

  virtual void accept(ExpressionVisitor<false> &visitor) = 0;

  virtual ~Expression() = 0;

  [[nodiscard]] std::unique_ptr<Expression> clone(
      const std::unordered_map<const Expression *, Expression *> &initial_map) const;

  // @brief
  // Returns true iff the 2 expressions are syntactically equal.
  // 2 expressions might reduce to the same normal form but not be equal in this
  // sense.
  [[nodiscard]] bool operator==(const Expression &other) const;

  // @brief
  // Returns the negation of `operator==`.
  [[nodiscard]] bool operator!=(const Expression &other) const;

  // @brief
  // Converts the current expression into a string
  [[nodiscard]] std::string to_string() const;

  [[nodiscard]] static std::shared_ptr<spdlog::logger> get_logger();

  std::vector<std::unique_ptr<Expression>> children;

 protected:
  Expression() = default;
};

inline std::ostream &operator<<(std::ostream &os, const Expression &expr) {
  return os << expr.to_string();
}

}  // namespace script3025

template <>
struct fmt::formatter<script3025::Expression> {
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }

  format_context::iterator format(const script3025::Expression &expr,
                                  format_context &ctx) const;
};

#endif
