#include <fstream>
#include <iostream>
#include <memory>

#include "expression/visitors/lazy_reduction_visitor.hpp"
#include "expression/visitors/type_gen_visitor.hpp"
#include "expression_factory.hpp"
#include "script3025_core.hpp"

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <script_file>" << std::endl;
    return 1;
  }
  std::ifstream script(argv[1]);
  std::string content((std::istreambuf_iterator<char>(script)),
                      (std::istreambuf_iterator<char>()));
  std::unique_ptr<script3025::Expression> expression =
      script3025::text_to_expression(content);
  script3025::TypeGenVisitor type_gen_visitor({}, {});

  expression->accept(type_gen_visitor);

  if (type_gen_visitor.get_type(expression.get()) == nullptr) {
    std::cerr << "Type generation failed." << std::endl;
    return 1;
  }

  std::cout << "Type of expression:" << std::endl
            << type_gen_visitor.get_type(expression.get())->to_string()
            << std::endl;

  auto reduced_type = script3025::reduce_copy(
      *type_gen_visitor.get_type((expression.get())), {});

  std::cout << "Reduced Type:" << std::endl
            << reduced_type->to_string() << std::endl;

  script3025::reduce_inplace(expression, {});

  std::cout << "Reduced expression:" << std::endl
            << expression->to_string() << std::endl;

  return 0;
}