#include "expression/expression_base.hpp"

#include <iomanip>
#include <unordered_map>

#include "expression/expression.hpp"
#include "expression/expression_visitor.hpp"
#include "expression/visitors/cloning_visitor.hpp"
#include "expression/visitors/equality_visitor.hpp"

namespace script3025 {

std::unique_ptr<Expression> Expression::clone(
    std::unordered_map<const Expression *, Expression *> initial_map) {
  CloningVisitor visitor;
  visitor.pointer_map = initial_map;
  accept(visitor);
  return std::move(visitor.value);
}

bool Expression::operator==(const Expression &rhs) const {
  EqualityVisitor visitor(this);
  rhs.accept(visitor);
  return visitor.get();
}

bool Expression::operator!=(const Expression &rhs) const {
  return !(*this == rhs);
}

Expression::~Expression() {}

std::shared_ptr<spdlog::logger> Expression::get_logger() {
  static std::shared_ptr<spdlog::logger> logger =
      ([&]() -> std::shared_ptr<spdlog::logger> {
        logger = spdlog::stderr_color_mt("script3025::Expression",
                                         spdlog::color_mode::always);
        logger->set_level(spdlog::level::warn);
        logger->set_pattern("%^[%l] [tid=%t] [%T.%F] [%s:%#] %v%$");
        return logger;
      })();
  return logger;
}

class DisambiguationVisitor : public ExpressionVisitor {
 public:
  void visit_application(const ApplicationExpression &e) {
    visit(*e.function());
    visit(*e.argument());
  }

  void visit_id(const IdExpression &e) {
    bool is_shadowed = false;
    for (ptrdiff_t i = name_stack.size() - 1; i >= 0; --i) {
      const Expression *source = name_stack[i];
      // Is this a match?
      if (source == e.source) {
        // Shadowing
        if (is_shadowed) {
          ambiguous_expressions.insert(e.source);
        }
        // Incorrect naming
        if (id_map[source] != e.id) {
          ambiguous_expressions.insert(e.source);
        }
        return;
      }
      if (id_map[source] == e.id) {
        is_shadowed = true;
      }
    }
    if (is_shadowed || e.source != nullptr) {
      failed_lookups.insert(&e);
    }
  }

  void visit_lambda(const LambdaExpression &e) {
    visit(*e.argument_type());
    id_map[&e] = e.argument_id;
    name_stack.push_back(&e);
    visit(*e.definition());
    name_stack.pop_back();
  }

  void visit_let(const LetExpression &e) {
    visit(*e.argument_type());
    visit(*e.argument_value());
    id_map[&e] = e.argument_id;
    name_stack.push_back(&e);
    visit(*e.definition());
    name_stack.pop_back();
  }

  void visit_pi(const PiExpression &e) {
    visit(*e.argument_type());
    id_map[&e] = e.argument_id;
    name_stack.push_back(&e);
    visit(*e.definition());
    name_stack.pop_back();
  }

  std::vector<const Expression *> name_stack;
  /** Which expressions are used in non-trivial ways? */
  std::unordered_set<const Expression *> ambiguous_expressions;
  /** Which expressions failed the lookup */
  std::unordered_set<const Expression *> failed_lookups;
  /** Pointer to id introduced by that pointer */
  std::unordered_map<const Expression *, std::string> id_map;

 protected:
  void visit_default(const Expression &e) {
    for (const std::unique_ptr<Expression> &child : e.children) {
      visit(*child);
    }
  }
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

class StringifyVisitor : public ExpressionVisitor {
 public:
  void visit_application(const ApplicationExpression &e) {
    const bool wrap =
        binding_power.application.left < min_binding_power_left_ ||
        binding_power.application.right < min_binding_power_right_;
    const size_t right_binding_power = min_binding_power_right_;
    const size_t left_binding_power = min_binding_power_left_;

    if (wrap) output << "(";

    min_binding_power_left_ = wrap ? 0 : left_binding_power;
    min_binding_power_right_ = binding_power.application.left + 1;
    visit(*e.function());

    output << " ";

    min_binding_power_left_ = binding_power.application.right + 1;
    min_binding_power_right_ = wrap ? 0 : right_binding_power;
    visit(*e.argument());

    if (wrap) output << ")";
  }

  void visit_equality(const EqualityExpression &e) {
    const bool wrap =
        binding_power.equality.left < min_binding_power_left_ ||
        binding_power.equality.right < min_binding_power_right_;
    const size_t right_binding_power = min_binding_power_right_;
    const size_t left_binding_power = min_binding_power_left_;

    if (wrap) output << "(";

    min_binding_power_left_ = wrap ? 0 : left_binding_power;
    min_binding_power_right_ = binding_power.equality.left + 1;
    visit(*e.lhs());

    output << "=";

    min_binding_power_left_ = binding_power.equality.right + 1;
    min_binding_power_right_ = wrap ? 0 : right_binding_power;
    visit(*e.rhs());

    if (wrap) output << ")";
  }

  void visit_id(const IdExpression &e) {
    // Did lookup fail?
    if (failed_lookups.find(&e) != failed_lookups.end()) {
      output << e.id << "[" << "0x" << std::setfill('0')
             << std::setw(sizeof(const IdExpression *) * 2) << std::hex
             << e.source << "]";
      return;
    }

    std::string displayed_id = e.id;

    // Did lookup succeed and hit the correct source?
    if (special_names.find(e.source) == special_names.end()) {
      output << displayed_id;
      return;
    }

    displayed_id = special_names[e.source];

    // Was that incorrect source named as expected?
    if (id_map[e.source] == e.id) {
      output << displayed_id;
      return;
    }

    displayed_id = e.id + "!" + displayed_id;
    output << displayed_id;
  }

  void visit_induction_keyword(const InductionKeywordExpression &e) {
    output << "induction";
  }

  void visit_lambda(const LambdaExpression &e) {
    const bool wrap = binding_power.equality.right < min_binding_power_right_;
    const size_t right_binding_power = min_binding_power_right_;
    const size_t left_binding_power = min_binding_power_left_;

    if (wrap) output << "(";

    output << "λ (" << e.argument_id << ":";

    min_binding_power_left_ = 0;
    min_binding_power_right_ = 0;
    visit(*e.argument_type());

    output << "). ";

    min_binding_power_left_ = binding_power.scope.right + 1;
    min_binding_power_right_ = wrap ? 0 : right_binding_power;
    visit(*e.definition());

    if (wrap) output << ")";
  }

  void visit_let(const LetExpression &e) {
    const bool wrap = binding_power.equality.right < min_binding_power_right_;
    const size_t right_binding_power = min_binding_power_right_;
    const size_t left_binding_power = min_binding_power_left_;

    if (wrap) output << "(";

    output << "let (" << e.argument_id << ":";

    min_binding_power_left_ = 0;
    min_binding_power_right_ = 0;
    visit(*e.argument_type());

    output << ":=";

    min_binding_power_left_ = 0;
    min_binding_power_right_ = 0;
    visit(*e.argument_value());

    output << " in ";

    min_binding_power_left_ = binding_power.scope.right + 1;
    min_binding_power_right_ = wrap ? 0 : right_binding_power;
    visit(*e.definition());

    if (wrap) output << ")";
  }

  void visit_nat_keyword(const NatKeywordExpression &e) { output << "Nat"; }

  void visit_nat_literal(const NatLiteralExpression &e) {
    output << e.value_.get_str();
  }

  void visit_pi(const PiExpression &e) {
    const bool wrap = binding_power.equality.right < min_binding_power_right_;
    const size_t right_binding_power = min_binding_power_right_;
    const size_t left_binding_power = min_binding_power_left_;

    if (wrap) output << "(";

    output << "Π (" << e.argument_id << ":";

    min_binding_power_left_ = 0;
    min_binding_power_right_ = 0;
    visit(*e.argument_type());

    output << "). ";

    min_binding_power_left_ = binding_power.scope.right + 1;
    min_binding_power_right_ = wrap ? 0 : right_binding_power;
    visit(*e.definition());

    if (wrap) output << ")";
  }

  void visit_replace_keyword(const ReplaceKeywordExpression &e) {
    output << "subst";
  }

  void visit_reflexive_keyword(const ReflexiveKeywordExpression &e) {
    output << "refl";
  }

  void visit_succ_keyword(const SuccKeywordExpression &e) { output << "succ"; }

  void visit_type_keyword(const TypeKeywordExpression &e) {

    const bool wrap =
        binding_power.application.left < min_binding_power_left_ ||
        binding_power.application.right < min_binding_power_right_;
    const size_t right_binding_power = min_binding_power_right_;
    const size_t left_binding_power = min_binding_power_left_;

    if (wrap) output << "(";
    output << "Type " << e.level_.get_str();
    if (wrap) output << ")";
  }

  std::unordered_map<const Expression *, std::string> special_names;
  std::unordered_map<const Expression *, std::string> id_map;
  std::unordered_set<const Expression *> failed_lookups;
  size_t min_binding_power_left_ = 0;
  size_t min_binding_power_right_ = 0;
  std::stringstream output;
};

std::string Expression::to_string() const {
  DisambiguationVisitor disambiguation_visitor;
  accept(disambiguation_visitor);

  /** The current disambiguation tag for each id */
  std::unordered_map<std::string, size_t> current_tag;
  /** The special names in the print */
  std::unordered_map<const Expression *, std::string> special_names;
  for (const Expression *e : disambiguation_visitor.ambiguous_expressions) {
    std::string id = disambiguation_visitor.id_map[e];
    current_tag.try_emplace(id, 0);
    ++current_tag[id];
    special_names.emplace(e, id + "#" + std::to_string(current_tag[id]));
  }

  StringifyVisitor stringify_visitor;
  stringify_visitor.special_names = std::move(special_names);
  stringify_visitor.id_map = std::move(disambiguation_visitor.id_map);
  stringify_visitor.failed_lookups =
      std::move(disambiguation_visitor.failed_lookups);
  accept(stringify_visitor);
  return stringify_visitor.output.str();
}

}  // namespace script3025

fmt::format_context::iterator fmt::formatter<script3025::Expression>::format(
    const script3025::Expression &expr, format_context &ctx) const {
  return fmt::format_to(ctx.out(), "{0}", expr.to_string());
}
