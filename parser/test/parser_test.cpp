#include <iostream>
#include <unordered_map>
#include <unordered_set>

#include <gtest/gtest.h>

#include "parser.hpp"

namespace parser {
  ParserBuilder<char> get_dyck_ruleset() {
    return ParserBuilder('G', '$')
      .rule('G')
      .rule('G', '(', 'G', ')', 'G', 'G');
  }
  
  ParserBuilder<char> get_arith_ruleset() {
    return ParserBuilder('E', '$')
      .rule('E', 'T')
      .rule('E', 'T', '+', 'E')
      .rule('T', 'F')
      .rule('T', 'F', '*', 'T')
      .rule('F', 'n')
      .rule('F', '(', 'E', ')');
  }

  ParserBuilder<char> get_parity_ruleset() {
    return ParserBuilder('E', '$')
      .rule('E')
      .rule('E', '0', 'E')
      .rule('E', '1', 'O')
      .rule('O', '0', 'O')
      .rule('O', '1', 'E');
  }

  ParserBuilder<char> get_dyck_ruleset_lr1() {
    return ParserBuilder('G', '$')
      .rule('G', 'D', 'G')
      .rule('G')
      .rule('D', '(', 'G', ')');
  }
  
  TEST(Build, dyck) {
    Parser<char> parser = get_dyck_ruleset_lr1().build();
    std::string my_text = "()()(())";
    parser.parse(my_text.begin(), my_text.end());
  }
}
