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

  TEST(EmptyTest, dyck) {
    ParserBuilder<char> parser = get_dyck_ruleset();

    parser.infer_types();
    auto empty_set = parser.get_empty_set();

    std::unordered_set<char> expected({'G'});

    EXPECT_EQ(empty_set, expected);
  }

  TEST(EmptyTest, arithmetic) {
    ParserBuilder<char> parser = get_arith_ruleset();

    parser.infer_types();
    auto empty_set = parser.get_empty_set();

    std::unordered_set<char> expected;

    EXPECT_EQ(empty_set, expected);
  }

  TEST(EmptyTest, parity) {
    ParserBuilder<char> parser = get_parity_ruleset();

    parser.infer_types();
    auto empty_set = parser.get_empty_set();

    std::unordered_set<char> expected({'E'});

    EXPECT_EQ(empty_set, expected);
  }

  TEST(FirstSet, dyck) {
    ParserBuilder<char> parser = get_dyck_ruleset();

    parser.infer_types();
    parser.get_empty_set();
    auto first_set = parser.get_first_set();

    std::unordered_map<char, std::unordered_set<char>> expected({
      {'G', std::unordered_set({'('})},
    });

    EXPECT_EQ(first_set, expected);
  }

  TEST(FirstSet, arithmetic) {
    ParserBuilder<char> parser = get_arith_ruleset();

    parser.infer_types();
    parser.get_empty_set();
    auto first_set = parser.get_first_set();

    std::unordered_map<char, std::unordered_set<char>> expected({
      {'E', std::unordered_set({'(', 'n'})},
      {'T', std::unordered_set({'(', 'n'})},
      {'F', std::unordered_set({'(', 'n'})},
    });

    EXPECT_EQ(first_set, expected);
  }

  TEST(FirstSet, parity) {
    ParserBuilder<char> parser = get_parity_ruleset();

    parser.infer_types();
    parser.get_empty_set();
    auto first_set = parser.get_first_set();

    std::unordered_map<char, std::unordered_set<char>> expected({
      {'E', std::unordered_set({'0', '1'})},
      {'O', std::unordered_set({'0', '1'})},
    });

    EXPECT_EQ(first_set, expected);
  }

  TEST(FollowSet, dyck) {
    ParserBuilder<char> parser = get_dyck_ruleset();

    parser.infer_types();
    parser.get_empty_set();
    parser.get_first_set();
    auto follow_set = parser.get_follow_set();

    std::unordered_map<char, std::unordered_set<char>> expected({
      {'G', std::unordered_set({'$', ')', '('})},
    });

    EXPECT_EQ(follow_set, expected);
  }

  TEST(FollowSet, arithmetic) {
    ParserBuilder<char> parser = get_arith_ruleset();

    parser.infer_types();
    parser.get_empty_set();
    parser.get_first_set();
    auto follow_set = parser.get_follow_set();

    std::unordered_map<char, std::unordered_set<char>> expected({
      {'E', std::unordered_set({'$', ')'})},
      {'T', std::unordered_set({'$', ')', '+'})},
      {'F', std::unordered_set({'$', ')', '+', '*'})},
    });

    EXPECT_EQ(follow_set, expected);
  }

  TEST(FollowSet, parity) {
    ParserBuilder<char> parser = get_parity_ruleset();

    parser.infer_types();
    parser.get_empty_set();
    parser.get_first_set();
    auto follow_set = parser.get_follow_set();

    std::unordered_map<char, std::unordered_set<char>> expected({
      {'E', std::unordered_set({'$'})},
      {'O', std::unordered_set({'$'})},
    });

    EXPECT_EQ(follow_set, expected);
  }
}
