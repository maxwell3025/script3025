#ifndef SCRIPT3025_SCRIPT3025_CORE_INDUCTIVE_DEFINITION_HPP
#define SCRIPT3025_SCRIPT3025_CORE_INDUCTIVE_DEFINITION_HPP

#include <memory>
#include <string>

#include "spdlog/spdlog.h"
#include "parser.hpp"

#include "expression/expression.hpp"

namespace script3025 {

class InductiveDefinition {
 public:
  template <typename Iterator>
  InductiveDefinition(parser::ConcreteSyntaxTree<Token> &source, Iterator &string_iterator);
 private:
  std::string name;
  std::unique_ptr<Expression> definition;
  static std::shared_ptr<spdlog::logger> get_logger();
};

template <typename Iterator>
InductiveDefinition::InductiveDefinition(parser::ConcreteSyntaxTree<Token> &source, Iterator &string_iterator) {
  // TODO
}

} // namespace script3025
#endif