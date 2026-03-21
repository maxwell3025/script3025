#include "expression_factory.hpp"

#include "cst_transformers.hpp"
#include "expression/visitors/normalizing_visitor.hpp"
#include "parsing_utility.hpp"

namespace script3025 {

[[nodiscard]] std::unique_ptr<Expression> text_to_expression(
    const std::string str) {
  ParsedCode code = text_to_expression_cst(str);

  script3025::collect_lists(*code.cst);
  script3025::collapse_oop(*code.cst);

  std::vector<std::string> token_text;
  for (const AnnotatedToken &token : code.annotated_tokens) {
    token_text.push_back(token.text);
  }

  auto iterator = token_text.begin();

  std::unique_ptr<Expression> expression =
      create_expression(*code.cst, iterator);

  NormalizingVisitor().visit(*expression);
  return expression;
}

}  // namespace script3025