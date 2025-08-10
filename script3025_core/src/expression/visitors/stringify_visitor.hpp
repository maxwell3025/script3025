#include <cstddef>
#include <iomanip>
#include <ios>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "expression/expression.hpp"
#include "expression/expression_visitor.hpp"
#include "expression/subtypes/scope_expression.hpp"

namespace script3025 {

namespace {

class IdRenamer : ConstExpressionVisitor {
 public:
  [[nodiscard]] static std::unique_ptr<IdRenamer> create(const Expression &e) {
    std::unique_ptr<IdRenamer> output(new IdRenamer());
    e.accept(*output);
    return output;
  }

  [[nodiscard]] std::string get_description(const IdExpression &e) const {
    std::stringstream output;
    bool is_global = e.source == nullptr;
    if (is_global) {
      if (shadowed_globals_.find(&e) != inaccessible_references_.end()) {
        output << e.id << "[global]";
      } else {
        output << e.id;
      }
      return output.str();
    }

    bool is_inaccessible =
        inaccessible_references_.find(&e) != inaccessible_references_.end();
    if (is_inaccessible) {
      output << e.id << "[" << "0x" << std::setfill('0')
             << std::setw(sizeof(const IdExpression *) * 2) << std::hex
             << e.source << "]";
      return output.str();
    }

    std::string variable_name;
    if (scope_rename_.find(e.source) != scope_rename_.end()) {
      variable_name = scope_rename_.at(e.source);
    } else {
      variable_name = e.source->argument_id;
    }
    output << variable_name;

    bool is_misnamed = e.source->argument_id != e.id;
    if (is_misnamed) {
      output << "!labeled as \"" << e.id << "\"!";
    }
    return output.str();
  }

 private:
  IdRenamer() = default;

  void visit_scope(const ScopeExpression &e) {
    for (const std::unique_ptr<Expression> &child : e.children) {
      if (child == e.definition()) scope_stack_.push_back(&e);
      visit(*child);
      if (child == e.definition()) scope_stack_.pop_back();
    }
  }

  void visit_id(const IdExpression &e) {
    bool is_shadowed = false;
    for (size_t i = scope_stack_.size(); i-- != 0;) {
      const ScopeExpression *local_scope = scope_stack_[i];
      if (local_scope == e.source) {
        if (is_shadowed) {
          shadowed_scopes_.insert(e.source);
        }
        return;
      }
      if (e.source != nullptr &&
          local_scope->argument_id == e.source->argument_id)
        is_shadowed = true;
    }

    bool is_global = e.source == nullptr;
    if (is_shadowed && is_global) shadowed_globals_.insert(&e);
    if (!is_global) inaccessible_references_.insert(&e);
  }

  void visit_expression(const Expression &e) {
    for (const std::unique_ptr<Expression> &child : e.children) {
      visit(*child);
    }
  }

  std::vector<const ScopeExpression *> scope_stack_;
  std::unordered_set<const ScopeExpression *> shadowed_scopes_;
  std::unordered_set<const IdExpression *> shadowed_globals_;
  std::unordered_set<const IdExpression *> inaccessible_references_;
  std::unordered_map<const ScopeExpression *, std::string> scope_rename_;
};

struct OperatorBindingPower {
  size_t left;
  size_t right;
};

struct BindingPower {
  OperatorBindingPower application{5, 6};
  OperatorBindingPower scope{0, 3};
  OperatorBindingPower equality{1, 1};
};

constexpr BindingPower binding_power;

/**
  Converts an Expression into a string using the minimum number of parentheses.

  This works via traversal.

  For `IdExpression` s where `source != nullptr`, `source` is treated as the
  source of truth.
  For `IdExpression` s where `source == nullptr`, `id` is treated as the source
  of truth.
  Crucially, when `source->argument_id` conflicts with `id`,
  `source->argument_id` wins.

  If an `IdExpression` references a variable that is shadowed (a more local
  scope conflicts the identifier), then a disambiguating suffix is added to that
  variable name.
  If `IdExpression::source` references a variable that is inaccessible (the
  `IdExpression` does not reference a global or any scope expression that is an
  ancestor of it), the expression is labeled with an address.
  If an `IdExpression` is misnamed (`IdExpression::id` conflicts with
  `IdExpression::source->argument_id`), the variable name of
  `IdExpression::source` is used, and the incorrect value of `id` is flagged.


  Global variables are referenced by nullptr.
  For any variables that are referred to in-scope, but past a shadow
  (e.g. a deeper scope reuses that id), a number is added.
  For any variables that are
 */
class ExpressionStringifier : ConstExpressionVisitor {
 public:
  ExpressionStringifier(std::unique_ptr<IdRenamer> renamer)
      : renamer_(std::move(renamer)) {}

  [[nodiscard]] std::string stringify(const Expression &e) {
    visit(e);
    return output_.str();
  }

 private:
  void visit_id(const IdExpression &e) {
    output_ << renamer_->get_description(e);
  }

  void visit_application(const ApplicationExpression &e) {
    const bool wrap =
        binding_power.application.left < min_binding_power_left_ ||
        binding_power.application.right < min_binding_power_right_;
    const size_t right_binding_power = min_binding_power_right_;
    const size_t left_binding_power = min_binding_power_left_;

    if (wrap) output_ << "(";

    min_binding_power_left_ = wrap ? 0 : left_binding_power;
    min_binding_power_right_ = binding_power.application.left + 1;
    visit(*e.function());

    output_ << " ";

    min_binding_power_left_ = binding_power.application.right + 1;
    min_binding_power_right_ = wrap ? 0 : right_binding_power;
    visit(*e.argument());

    if (wrap) output_ << ")";
  }

  void visit_equality(const EqualityExpression &e) {
    const bool wrap = binding_power.equality.left < min_binding_power_left_ ||
                      binding_power.equality.right < min_binding_power_right_;
    const size_t right_binding_power = min_binding_power_right_;
    const size_t left_binding_power = min_binding_power_left_;

    if (wrap) output_ << "(";

    min_binding_power_left_ = wrap ? 0 : left_binding_power;
    min_binding_power_right_ = binding_power.equality.left + 1;
    visit(*e.lhs());

    output_ << "=";

    min_binding_power_left_ = binding_power.equality.right + 1;
    min_binding_power_right_ = wrap ? 0 : right_binding_power;
    visit(*e.rhs());

    if (wrap) output_ << ")";
  }

  void visit_induction_keyword(const InductionKeywordExpression &) {
    output_ << "induction";
  }

  void visit_lambda(const LambdaExpression &e) {
    const bool wrap = binding_power.equality.right < min_binding_power_right_;
    const size_t right_binding_power = min_binding_power_right_;
    // const size_t left_binding_power = min_binding_power_left_;

    if (wrap) output_ << "(";

    output_ << "λ (" << e.argument_id << ":";

    min_binding_power_left_ = 0;
    min_binding_power_right_ = 0;
    visit(*e.argument_type());

    output_ << "). ";

    min_binding_power_left_ = binding_power.scope.right + 1;
    min_binding_power_right_ = wrap ? 0 : right_binding_power;
    visit(*e.definition());

    if (wrap) output_ << ")";
  }

  void visit_let(const LetExpression &e) {
    const bool wrap = binding_power.equality.right < min_binding_power_right_;
    const size_t right_binding_power = min_binding_power_right_;
    // const size_t left_binding_power = min_binding_power_left_;

    if (wrap) output_ << "(";

    output_ << "let (" << e.argument_id << ":";

    min_binding_power_left_ = 0;
    min_binding_power_right_ = 0;
    visit(*e.argument_type());

    output_ << ":=";

    min_binding_power_left_ = 0;
    min_binding_power_right_ = 0;
    visit(*e.argument_value());

    output_ << " in ";

    min_binding_power_left_ = binding_power.scope.right + 1;
    min_binding_power_right_ = wrap ? 0 : right_binding_power;
    visit(*e.definition());

    if (wrap) output_ << ")";
  }

  void visit_nat_keyword(const NatKeywordExpression &) { output_ << "Nat"; }

  void visit_nat_literal(const NatLiteralExpression &e) {
    output_ << e.value.get_str();
  }

  void visit_pi(const PiExpression &e) {
    const bool wrap = binding_power.equality.right < min_binding_power_right_;
    const size_t right_binding_power = min_binding_power_right_;
    // const size_t left_binding_power = min_binding_power_left_;

    if (wrap) output_ << "(";

    output_ << "Π (" << e.argument_id << ":";

    min_binding_power_left_ = 0;
    min_binding_power_right_ = 0;
    visit(*e.argument_type());

    output_ << "). ";

    min_binding_power_left_ = binding_power.scope.right + 1;
    min_binding_power_right_ = wrap ? 0 : right_binding_power;
    visit(*e.definition());

    if (wrap) output_ << ")";
  }

  void visit_replace_keyword(const ReplaceKeywordExpression &) {
    output_ << "subst";
  }

  void visit_reflexive_keyword(const ReflexiveKeywordExpression &) {
    output_ << "refl";
  }

  void visit_succ_keyword(const SuccKeywordExpression &) { output_ << "succ"; }

  void visit_type_keyword(const TypeKeywordExpression &e) {
    const bool wrap =
        binding_power.application.left < min_binding_power_left_ ||
        binding_power.application.right < min_binding_power_right_;
    // const size_t right_binding_power = min_binding_power_right_;
    // const size_t left_binding_power = min_binding_power_left_;

    if (wrap) output_ << "(";
    output_ << "Type " << e.level.get_str();
    if (wrap) output_ << ")";
  }

  std::unique_ptr<IdRenamer> renamer_;
  std::stringstream output_;
  size_t min_binding_power_left_ = 0;
  size_t min_binding_power_right_ = 0;
};

}  // namespace

inline std::string stringify_expression(const Expression &e) {
  std::unique_ptr<IdRenamer> id_renamer = IdRenamer::create(e);
  ExpressionStringifier stringifier(std::move(id_renamer));
  return stringifier.stringify(e);
}

}  // namespace script3025