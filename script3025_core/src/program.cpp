#include "program.hpp"

#include <sstream>

#include "cst_transformers.hpp"
#include "expression/visitors/scope_hygiene_visitor.hpp"
#include "parsing_utility.hpp"

namespace script3025 {

Program::Program(const std::string &source) {
  ParsedCode code = parse(source);

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
  for (const auto &definition_pair : global_definitions())
    if (!is_hygenic(*definition_pair.second)) return false;
  return true;
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
