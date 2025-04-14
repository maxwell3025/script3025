#ifndef SCRIPT3025_SCRIPT3025_CORE_SCRIPT3025_CORE_HPP
#define SCRIPT3025_SCRIPT3025_CORE_SCRIPT3025_CORE_HPP

#include <string>
#include <unordered_map>

#include "parser.hpp"
#include "parsing_utility.hpp"

namespace script3025 {

class Definition {

};

class InductiveDefinition : public Definition {

};

class NormalDefinition : public Definition {

};

class Value {

};

class Environment;

class ValueExpression {
 public:
 private:
  Environment *environment;
};

class Environment {
 public:
  Environment(parser::ConcreteSyntaxTree<Token> &source);
  
 private:
  std::unordered_map<std::string, ValueExpression> definitions;
  Environment *parent;
};

void collect_lists(parser::ConcreteSyntaxTree<Token> &tree);

void collapse_oop(parser::ConcreteSyntaxTree<Token> &tree);

}

#endif
