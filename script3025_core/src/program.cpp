#include "program.hpp"

#include "parsing_utility.hpp"
#include "cst_transformers.hpp"

namespace script3025 {

Program::Program(std::string source) {
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
