#include "program.hpp"

#include <cstddef>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "cst_transformers.hpp"
#include "expression/expression.hpp"
#include "expression/variable_reference.hpp"
#include "expression/visitors/lazy_reduction_visitor.hpp"
#include "expression/visitors/scope_hygiene_visitor.hpp"
#include "expression/visitors/type_gen_visitor.hpp"
#include "parsing_utility.hpp"
#include "spdlog/common.h"
#include "spdlog/logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

namespace script3025 {

Program::Program(const std::string &source) {
  ParsedCode code = text_to_program_cst(source);

  script3025::collect_lists(*code.cst);
  script3025::collapse_oop(*code.cst);

  std::vector<std::string> token_text;
  for (const AnnotatedToken &token : code.annotated_tokens) {
    token_text.push_back(token.text);
  }

  auto iterator = token_text.begin();
  add_program(*code.cst, iterator);
}

std::string Program::to_string() const {
  std::stringstream output;
  for (size_t i = 0; i < global_ids().size(); ++i) {
    const std::string &id = global_ids()[i];
    output << "def " << id << ":=" << global(id);
    if (i < global_ids().size() - 1) output << std::endl;
  }
  return output.str();
}

bool Program::check_types() const {
  TypeGenVisitor type_visitor{{}, {}};
  for (const std::string &definition_name : global_ids()) {
    const std::unique_ptr<Expression> &definition =
        global_definitions_.at(definition_name);
    if (!is_hygenic(*definition)) return false;

    type_visitor.visit(*definition);
    Expression *type_expression_ptr = type_visitor.get_type(definition.get());
    if (type_expression_ptr == nullptr) {
      return false;
    }

    type_visitor.bind_global(definition.get(), definition_name);
    SPDLOG_LOGGER_TRACE(get_logger(),
                        "Evaluated type for global variable.\n"
                        "{}: {}",
                        definition_name,
                        type_visitor.get_type(definition_name)->to_string());
  }
  return true;
}

[[nodiscard]] std::unique_ptr<Expression> Program::reduce(
    const std::string &id) {
  std::unordered_map<VariableReference, const Expression *> delta_table;

  size_t const n_globals = id_ordering_[id];
  for (size_t i = 0; i < n_globals; ++i) {
    std::string const name = global_ids_[i];
    delta_table.emplace(VariableReference{name, nullptr}, &global(name));
  }
  return script3025::reduce_copy(*global_definitions_[id], delta_table);
}

std::shared_ptr<spdlog::logger> Program::get_logger() {
  static std::shared_ptr<spdlog::logger> logger =
      ([&]() -> std::shared_ptr<spdlog::logger> {
        logger = spdlog::stderr_color_mt("script3025::Program",
                                         spdlog::color_mode::always);
        logger->set_level(spdlog::level::warn);
        logger->set_pattern("%^[%l] [tid=%t] [%T.%F] [%s:%#] %v%$");
        return logger;
      })();
  return logger;
}

}  // namespace script3025
