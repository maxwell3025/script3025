#include <unordered_map>
#include <unordered_set>

#include <gtest/gtest.h>

#include "parser.hpp"

namespace parser {
  TotalRuleset<char> get_dyck_ruleset() {
    TotalRuleset<char> ruleset;
    ruleset['G'] = std::unordered_set<std::vector<char>, VectorHash<char>>({
      std::vector<char>(),
      std::vector<char>({'(', 'G', ')', 'G'})
    });
    return ruleset;
  }
  
  TotalRuleset<char> get_arith_ruleset() {
    TotalRuleset<char> ruleset;
    ruleset['E'] = std::unordered_set<std::vector<char>, VectorHash<char>>({
      std::vector<char>({'T'}),
      std::vector<char>({'T', '+', 'E'})
    });
    ruleset['T'] = std::unordered_set<std::vector<char>, VectorHash<char>>({
      std::vector<char>({'F'}),
      std::vector<char>({'F', '*', 'T'}),
    });
    ruleset['F'] = std::unordered_set<std::vector<char>, VectorHash<char>>({
      std::vector<char>({'n'}),
      std::vector<char>({'(', 'E', ')'}),
    });
    return ruleset;
  }

  TotalRuleset<char> get_parity_ruleset() {
    TotalRuleset<char> ruleset;
    ruleset['E'] = std::unordered_set<std::vector<char>, VectorHash<char>>({
      std::vector<char>({}),
      std::vector<char>({'0', 'E'}),
      std::vector<char>({'1', 'O'})
    });
    ruleset['O'] = std::unordered_set<std::vector<char>, VectorHash<char>>({
      std::vector<char>({'0', 'O'}),
      std::vector<char>({'1', 'E'})
    });
    return ruleset;
  }

  TEST(EmptyTest, dyck) {
    TotalRuleset<char> ruleset = get_dyck_ruleset();

    std::unordered_set<char> nonterminals = std::unordered_set({
      'G'
    });

    std::unordered_set<char> empty_set = get_empty_set(nonterminals, ruleset);

    std::unordered_set<char> expected({'G'});

    EXPECT_EQ(empty_set, expected);
  }

  TEST(EmptyTest, arithmetic) {
    TotalRuleset<char> ruleset = get_arith_ruleset();

    std::unordered_set<char> nonterminals = std::unordered_set({
      'E',
      'T',
      'F'
    });

    std::unordered_set<char> empty_set = get_empty_set(nonterminals, ruleset);

    std::unordered_set<char> expected;

    EXPECT_EQ(empty_set, expected);
  }

  TEST(EmptyTest, parity) {
    TotalRuleset<char> ruleset = get_parity_ruleset();

    std::unordered_set<char> nonterminals = std::unordered_set({
      'E',
      'O',
    });

    std::unordered_set<char> empty_set = get_empty_set(nonterminals, ruleset);

    std::unordered_set<char> expected({'E'});

    EXPECT_EQ(empty_set, expected);
  }

  TEST(FirstSet, dyck) {
    TotalRuleset<char> ruleset = get_dyck_ruleset();

    std::unordered_set<char> nonterminals = std::unordered_set({
      'G'
    });

    std::unordered_set<char> empty_set = get_empty_set(nonterminals, ruleset);

    std::unordered_map<char, std::unordered_set<char>> first_set = get_first_set(ruleset, nonterminals, empty_set);

    std::unordered_map<char, std::unordered_set<char>> expected({
      {'G', std::unordered_set({'('})},
    });

    EXPECT_EQ(first_set, expected);
  }

  TEST(FirstSet, arithmetic) {
    TotalRuleset<char> ruleset = get_arith_ruleset();

    std::unordered_set<char> nonterminals = std::unordered_set({
      'E',
      'T',
      'F'
    });

    std::unordered_set<char> empty_set = get_empty_set(nonterminals, ruleset);

    std::unordered_map<char, std::unordered_set<char>> first_set = get_first_set(ruleset, nonterminals, empty_set);

    std::unordered_map<char, std::unordered_set<char>> expected({
      {'E', std::unordered_set({'(', 'n'})},
      {'T', std::unordered_set({'(', 'n'})},
      {'F', std::unordered_set({'(', 'n'})},
    });

    EXPECT_EQ(first_set, expected);
  }

  TEST(FirstSet, parity) {
    TotalRuleset<char> ruleset = get_parity_ruleset();

    std::unordered_set<char> nonterminals = std::unordered_set({
      'E',
      'O',
    });

    std::unordered_set<char> empty_set = get_empty_set(nonterminals, ruleset);

    std::unordered_map<char, std::unordered_set<char>> first_set = get_first_set(ruleset, nonterminals, empty_set);

    std::unordered_map<char, std::unordered_set<char>> expected({
      {'E', std::unordered_set({'0', '1'})},
      {'O', std::unordered_set({'0', '1'})},
    });

    EXPECT_EQ(first_set, expected);
  }

  TEST(FollowSet, dyck) {
    TotalRuleset<char> ruleset = get_dyck_ruleset();

    std::unordered_set<char> nonterminals = std::unordered_set({
      'G'
    });

    std::unordered_set<char> empty_set = get_empty_set(nonterminals, ruleset);

    std::unordered_map<char, std::unordered_set<char>> first_set = get_first_set(ruleset, nonterminals, empty_set);

    std::unordered_map<char, std::unordered_set<char>> follow_set = get_follow_set(
        'G',
        '$',
        ruleset,
        nonterminals,
        empty_set,
        first_set);

    std::unordered_map<char, std::unordered_set<char>> expected({
      {'G', std::unordered_set({'$', ')'})},
    });

    EXPECT_EQ(follow_set, expected);
  }

  TEST(FollowSet, arithmetic) {
    TotalRuleset<char> ruleset = get_arith_ruleset();

    std::unordered_set<char> nonterminals = std::unordered_set({
      'E',
      'T',
      'F'
    });

    std::unordered_set<char> empty_set = get_empty_set(nonterminals, ruleset);

    std::unordered_map<char, std::unordered_set<char>> first_set = get_first_set(ruleset, nonterminals, empty_set);

    std::unordered_map<char, std::unordered_set<char>> follow_set = get_follow_set(
        'E',
        '$',
        ruleset,
        nonterminals,
        empty_set,
        first_set);

    std::unordered_map<char, std::unordered_set<char>> expected({
      {'E', std::unordered_set({'$', ')'})},
      {'T', std::unordered_set({'$', ')', '+'})},
      {'F', std::unordered_set({'$', ')', '+', '*'})},
    });

    EXPECT_EQ(follow_set, expected);
  }

  TEST(FollowSet, parity) {
    TotalRuleset<char> ruleset = get_parity_ruleset();

    std::unordered_set<char> nonterminals = std::unordered_set({
      'E',
      'O',
    });

    std::unordered_set<char> empty_set = get_empty_set(nonterminals, ruleset);

    std::unordered_map<char, std::unordered_set<char>> first_set = get_first_set(ruleset, nonterminals, empty_set);

    std::unordered_map<char, std::unordered_set<char>> follow_set = get_follow_set(
        'E',
        '$',
        ruleset,
        nonterminals,
        empty_set,
        first_set);

    std::unordered_map<char, std::unordered_set<char>> expected({
      {'E', std::unordered_set({'$'})},
      {'O', std::unordered_set({'$'})},
    });

    EXPECT_EQ(follow_set, expected);
  }
}
