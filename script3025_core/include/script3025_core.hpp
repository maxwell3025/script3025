#ifndef SCRIPT3025_CORE_HPP
#define SCRIPT3025_CORE_HPP

#include <string>
#include <unordered_map>

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
  
 private:
  std::unordered_map<std::string, ValueExpression> definitions;
  Environment *parent;
};

}

#endif
