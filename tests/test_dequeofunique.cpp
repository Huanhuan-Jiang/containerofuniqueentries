#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <compare>
#include <concepts>
#include <deque>
#include <numeric>
#include <stdexcept>
#include <unordered_set>
#include <utility>
#include <vector>

#include "dequeofunique.h"

TEST(DequeOfUniqueTest, DefaultConstructor) {
  containerofunique::deque_of_unique<int> dou;
  std::deque<int> emptydq;
  std::unordered_set<int> emptyset;

  EXPECT_EQ(dou.deque(), emptydq);
  EXPECT_THAT(dou.set(), ::testing::UnorderedElementsAreArray(emptyset));
}

TEST(DequeOfUniqueTest, ConstructorInitializesFromIterators) {
  std::deque<int> dq = {1, 2, 3, 4, 5};
  containerofunique::deque_of_unique<int> dou(dq.begin(), dq.end());

  EXPECT_EQ(dou.deque(), dq);
  EXPECT_THAT(std::deque<int>(dou.set().begin(), dou.set().end()),
              ::testing::UnorderedElementsAreArray(dq));
}

TEST(DequeOfUniqueTest, ConstructorWithInitializerListChecksDequeAndSet) {
  containerofunique::deque_of_unique<int> dou1 = {1};
  containerofunique::deque_of_unique<int> dou2 = {1, 2};
  containerofunique::deque_of_unique<int> dou3 = {1, 2, 3,
                                                  3};  // duplicate elements

  std::deque<int> dq1 = {1};
  std::deque<int> dq2 = {1, 2};
  std::deque<int> dq3 = {1, 2, 3};

  EXPECT_EQ(dou1.deque(), dq1);
  EXPECT_EQ(dou2.deque(), dq2);
  EXPECT_EQ(dou3.deque(), dq3);

  EXPECT_THAT(std::deque<int>(dou1.set().begin(), dou1.set().end()),
              ::testing::UnorderedElementsAreArray(dq1));
  EXPECT_THAT(std::deque<int>(dou2.set().begin(), dou2.set().end()),
              ::testing::UnorderedElementsAreArray(dq2));
  EXPECT_THAT(std::deque<int>(dou3.set().begin(), dou3.set().end()),
              ::testing::UnorderedElementsAreArray(dq3));
}

TEST(DequeOfUniqueTest, CopyConstructor_EmptyDeque) {
  containerofunique::deque_of_unique<int> dou1;
  // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
  containerofunique::deque_of_unique<int> dou2(dou1);
  EXPECT_TRUE(dou2.deque().empty());
  EXPECT_TRUE(dou2.set().empty());
}

TEST(DequeOfUniqueTest, CopyConstructor_SingleElement) {
  containerofunique::deque_of_unique<int> dou1 = {42};
  // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
  containerofunique::deque_of_unique<int> dou2(dou1);
  std::deque<int> dq = {42};
  EXPECT_EQ(dou2.deque(), dq);
  EXPECT_THAT(dou2.set(), ::testing::UnorderedElementsAreArray(dq));
}

TEST(DequeOfUniqueTest, CopyConstructor_Independence) {
  containerofunique::deque_of_unique<int> dou1 = {1, 2, 3};
  containerofunique::deque_of_unique<int> dou2(dou1);

  dou1.push_back(4);  // Modify the original
  EXPECT_EQ(dou1.deque(), std::deque<int>({1, 2, 3, 4}));
  EXPECT_EQ(dou2.deque(), std::deque<int>({1, 2, 3}));
}

TEST(DequeOfUniqueTest, CopyConstructor_LargeData) {
  std::deque<int> large_data(1000);
  // NOLINTNEXTLINE(modernize-use-std-ranges, modernize-use-algorith)
  std::iota(large_data.begin(), large_data.end(), 0);

  // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
  containerofunique::deque_of_unique<int> dou1(large_data.begin(),
                                               large_data.end());
  // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
  containerofunique::deque_of_unique<int> dou2(dou1);

  EXPECT_EQ(dou1.deque(), dou2.deque());
  EXPECT_THAT(dou2.set(), ::testing::UnorderedElementsAreArray(large_data));
}

TEST(DequeOfUniqueTest, MoveConstructor) {
  containerofunique::deque_of_unique<int> dou1 = {1, 2, 3, 4};
  containerofunique::deque_of_unique<int> dou2(std::move(dou1));
  std::deque<int> dq = {1, 2, 3, 4};
  EXPECT_EQ(dou2.deque(), dq);
}

TEST(DequeOfUniqueTest, CopyAssignmentOperator) {
  containerofunique::deque_of_unique<int> dou1 = {1, 2, 3, 4};
  containerofunique::deque_of_unique<int> dou2 = dou1;
  std::deque<int> dq = {1, 2, 3, 4};

  EXPECT_EQ(dou2.deque(), dou1.deque());
  EXPECT_THAT(std::deque<int>(dou2.set().begin(), dou2.set().end()),
              ::testing::UnorderedElementsAreArray(dq));
  dou1.push_back(
      5);  // This is used to suppress warning of
           // [performance-unnecessary-copy-initialization,-warnings-as-errors]
}

TEST(DequeOfUniqueTest, MoveAssignmentOperator) {
  containerofunique::deque_of_unique<int> dou1 = {1, 2, 3, 4};
  containerofunique::deque_of_unique<int> dou2 = std::move(dou1);
  std::deque<int> dq = {1, 2, 3, 4};

  EXPECT_EQ(dou2.deque(), dq);
  EXPECT_THAT(std::deque<int>(dou2.set().begin(), dou2.set().end()),
              ::testing::UnorderedElementsAreArray(dq));
}

TEST(DequeOfUniqueTest, InitializerListAssignmentOperator) {
  containerofunique::deque_of_unique<int> dou = {1, 2, 3, 4};
  std::deque<int> dq = {1, 2, 3, 4};
  EXPECT_EQ(dou.deque(), dq);
  EXPECT_THAT(std::deque<int>(dou.set().begin(), dou.set().end()),
              ::testing::UnorderedElementsAreArray(dq));
}

TEST(DequeOfUniqueTest, ElementAccess) {
  containerofunique::deque_of_unique<int> dou = {1, 2, 3, 4};
  EXPECT_EQ(dou.front(), 1);
  EXPECT_EQ(dou.at(1), 2);
  EXPECT_EQ(dou[2], 3);
  EXPECT_EQ(dou.back(), 4);
}

TEST(DequeOfUniqueTest, At_OutOfRange) {
  containerofunique::deque_of_unique<int> dou = {1, 2, 3, 4};
  const containerofunique::deque_of_unique<std::string> dou_const = {"hello",
                                                                     "world"};
  EXPECT_THROW(dou.at(4), std::out_of_range);
  EXPECT_THROW(dou_const.at(2), std::out_of_range);
}

TEST(DequeOfUniqueTest, ElementAccess_ConstDeque) {
  const containerofunique::deque_of_unique<std::string> dou = {"hello",
                                                               "world"};
  EXPECT_EQ(dou.front(), "hello");
  EXPECT_EQ(dou.at(0), "hello");
  EXPECT_EQ(dou.at(1), "world");
  EXPECT_EQ(dou[0], "hello");
  EXPECT_EQ(dou[1], "world");
  EXPECT_EQ(dou.back(), "world");
}

// Test for normal iteration using cbegin() and cend()
TEST(DequeOfUniqueTest, CbeginCend_Iteration) {
  containerofunique::deque_of_unique<int> dou = {1, 2, 3, 4};

  auto it = dou.cbegin();
  EXPECT_EQ(*it, 1);
  ++it;
  EXPECT_EQ(*it, 2);
  ++it;
  EXPECT_EQ(*it, 3);
  ++it;
  EXPECT_EQ(*it, 4);
  ++it;
  EXPECT_EQ(it, dou.cend());  // Ensure iterator reaches cend()
}

// Test for normal iteration using crbegin() and crend()
TEST(DequeOfUniqueTest, CrbeginCrend_Iteration) {
  containerofunique::deque_of_unique<int> dou = {1, 2, 3, 4};

  auto rit = dou.crbegin();
  EXPECT_EQ(*rit, 4);
  ++rit;
  EXPECT_EQ(*rit, 3);
  ++rit;
  EXPECT_EQ(*rit, 2);
  ++rit;
  EXPECT_EQ(*rit, 1);
  ++rit;
  EXPECT_EQ(rit, dou.crend());  // Ensure reverse iterator reaches crend()
}

// Test for empty container's iterators
TEST(DequeOfUniqueTest, EmptyContainer_Iterators) {
  containerofunique::deque_of_unique<int> empty_dou;

  // For an empty deque, cbegin() should be equal to cend()
  EXPECT_EQ(empty_dou.cbegin(), empty_dou.cend());
  // For an empty deque, crbegin() should be equal to crend()
  EXPECT_EQ(empty_dou.crbegin(), empty_dou.crend());
}

// Test for const-correctness of iterators
TEST(DequeOfUniqueTest, ConstCorrectness_Iterators) {
  containerofunique::deque_of_unique<int> dou = {1, 2, 3, 4};

  EXPECT_TRUE((std::same_as<decltype(*dou.cbegin()), const int &>));
  EXPECT_TRUE((std::same_as<decltype(*dou.cend()), const int &>));
  EXPECT_TRUE((std::same_as<decltype(*dou.crbegin()), const int &>));
  EXPECT_TRUE((std::same_as<decltype(*dou.crend()), const int &>));
}

// Test that iterators do not modify elements (compile-time check)
TEST(DequeOfUniqueTest, Iterator_ModificationNotAllowed) {
  containerofunique::deque_of_unique<int> dou = {1, 2, 3, 4};
  auto const_it = dou.cbegin();
  ASSERT_EQ(*const_it, 1);
  ASSERT_TRUE(std::is_const_v<std::remove_reference_t<decltype(*const_it)>>);
}

TEST(DequeOfUniqueTest, Clear) {
  containerofunique::deque_of_unique<int> dou = {1, 2, 3, 4, 5};
  dou.clear();

  EXPECT_EQ(dou.deque().size(), 0);  // Deque should be empty
  EXPECT_THAT(dou.set(),
              ::testing::UnorderedElementsAre());  // Set should be empty
}

TEST(DequeOfUniqueTest, Erase_SingleElement) {
  containerofunique::deque_of_unique<int> dou = {1, 2, 3, 4, 5};
  std::deque<int> expected_deque = {2, 3, 4, 5};
  std::unordered_set<int> expected_set = {2, 3, 4, 5};

  dou.erase(dou.cbegin());
  EXPECT_EQ(dou.deque(), expected_deque);
  EXPECT_THAT(dou.set(), ::testing::UnorderedElementsAreArray(expected_set));
}

TEST(DequeOfUniqueTest, Erase_FromEmptyContainer) {
  containerofunique::deque_of_unique<int> dou;
  EXPECT_NO_THROW(dou.erase(dou.cbegin()));
  EXPECT_EQ(dou.deque().size(), 0);
}

TEST(DequeOfUniqueTest, EraseEmptyRange) {
  containerofunique::deque_of_unique<int> dou1 = {1, 2, 3, 4, 5, 6};
  std::deque<int> dq2 = {1, 2, 3, 4, 5, 6};
  std::unordered_set<int> set2 = {1, 2, 3, 4, 5, 6};

  auto result = dou1.erase(dou1.cbegin(), dou1.cbegin());
  EXPECT_EQ(result, dou1.cbegin());
  EXPECT_EQ(dou1.deque(), dq2);
  EXPECT_THAT(dou1.set(), ::testing::UnorderedElementsAreArray(set2));
}

TEST(DequeOfUniqueTest, EraseRangeOfElements) {
  containerofunique::deque_of_unique<int> dou1 = {1, 2, 3, 4, 5, 6};
  std::deque<int> dq2 = {4, 5, 6};
  std::unordered_set<int> set2 = {4, 5, 6};

  dou1.erase(dou1.cbegin(), dou1.cbegin() + 3);
  EXPECT_EQ(dou1.deque(), dq2);
  EXPECT_THAT(dou1.set(), ::testing::UnorderedElementsAreArray(set2));
}

TEST(DequeOfUniqueTest, EraseAllElements) {
  containerofunique::deque_of_unique<int> dou1 = {1, 2, 3, 4, 5, 6};
  std::deque<int> dq2 = {};
  std::unordered_set<int> set2 = {};

  auto result = dou1.erase(dou1.cbegin(), dou1.cend());
  EXPECT_EQ(result, dou1.cend());
  EXPECT_EQ(dou1.deque(), dq2);
  EXPECT_THAT(dou1.set(), ::testing::UnorderedElementsAreArray(set2));
}

TEST(DequeOfUniqueTest, InsertLvalueRvalue) {
  std::cout << "Test inserting a unique element" << '\n';
  containerofunique::deque_of_unique<int> dou1 = {1};
  std::deque<int> dq1 = {1};
  auto result1 = dou1.insert(dou1.cbegin(), 2);
  auto expected_result1 = dq1.insert(dq1.cbegin(), 2);
  EXPECT_EQ(*result1.first, *expected_result1);
  EXPECT_TRUE(result1.second);

  std::cout << "Test inserting a duplicate element" << '\n';
  containerofunique::deque_of_unique<int> dou2 = {1};
  std::deque<int> dq2 = {1, 2};
  auto result2 = dou2.insert(dou2.cbegin(), 1);
  EXPECT_EQ(*result2.first, *dou2.cbegin());
  EXPECT_FALSE(result2.second);

  std::cout << "Test inserting a unique rvalue string element" << '\n';
  containerofunique::deque_of_unique<std::string> dou3 = {"hello", "world"};
  std::deque<std::string> dq3 = {"hello", "world"};
  std::string str1 = "good";
  auto expected_result3 = dq3.insert(dq3.cbegin(), std::move("good"));
  auto result3 = dou3.insert(dou3.cbegin(), std::move(str1));
  EXPECT_EQ(dou3.deque(), (std::deque<std::string>{"good", "hello", "world"}));
  EXPECT_EQ(*result3.first, *expected_result3);
  EXPECT_TRUE(result3.second);

  std::cout << "Test inserting a duplicate rvalue string element" << '\n';
  containerofunique::deque_of_unique<std::string> dou4 = {"hello", "world"};
  std::deque<std::string> dq4 = {"hello", "world"};
  std::string str2 = "hello";
  auto result4 = dou4.insert(dou4.cbegin(), std::move(str2));
  EXPECT_EQ(dou4.deque(), dq4);
  EXPECT_EQ(*result4.first, *dou4.cbegin());
  EXPECT_FALSE(result4.second);
}

TEST(DequeOfUniqueTest, InsertRangeTest) {
  containerofunique::deque_of_unique<std::string> dou5_1 = {"hello", "world"};
  containerofunique::deque_of_unique<std::string> dou5_2 = {"good", "morning"};
  containerofunique::deque_of_unique<std::string> dou5_3 = {"hello", "world"};
  std::deque<std::string> dq5 = {"good", "morning", "hello", "world"};
  auto result5_1 =
      dou5_1.insert(dou5_1.cbegin(), dou5_2.cbegin(), dou5_2.cbegin() + 2);
  EXPECT_EQ(dou5_1.deque(), dq5);
  EXPECT_EQ(*result5_1, *dou5_1.cbegin());
  auto result5_2 =
      dou5_1.insert(dou5_1.cbegin(), dou5_3.cbegin(), dou5_3.cbegin() + 2);
  EXPECT_EQ(dou5_1.deque(), dq5);
  EXPECT_EQ(*result5_2, *(dou5_1.cbegin()));

  containerofunique::deque_of_unique<std::string> dou6 = {"hello", "world"};
  std::deque<std::string> dq6 = {"good", "morning", "hello", "world"};
  auto result6_1 = dou6.insert(dou6.cbegin(), {"good", "morning"});
  EXPECT_EQ(dou6.deque(), dq6);
  EXPECT_EQ(*result6_1, *dou6.cbegin());
  auto result6_2 = dou6.insert(dou6.cbegin(), {"good", "morning"});
  EXPECT_EQ(dou6.deque(), dq6);
  EXPECT_EQ(*result6_2, *dou6.cbegin());
}

TEST(DequeOfUniqueTest, InsertEmptyRange) {
  containerofunique::deque_of_unique<std::string> dou1 = {"existing"};
  std::deque<std::string> dq1 = {"existing"};
  std::deque<std::string> dq2 = {"hello", "world", "apple", "fruit"};

  auto result1 = dou1.insert(dou1.cbegin(), dq2.begin(), dq2.begin());
  EXPECT_EQ(result1, dou1.cbegin());
  EXPECT_EQ(dou1.deque(), dq1);

  auto result2 = dou1.insert(dou1.cbegin(), {});
  EXPECT_EQ(result2, dou1.cbegin());
  EXPECT_EQ(dou1.deque(), dq1);
}

TEST(DequeOfUniqueTest, InsertAtEnd) {
  containerofunique::deque_of_unique<std::string> dou = {"hello"};
  auto result = dou.insert(dou.cend(), "world");
  EXPECT_EQ(*result.first, dou.back());
  EXPECT_EQ(result.second, true);
  EXPECT_EQ(dou.deque(), (std::deque<std::string>{"hello", "world"}));
}

TEST(DequeOfUniqueTest, InsertAtBeginning) {
  containerofunique::deque_of_unique<std::string> dou = {"world"};
  auto result = dou.insert(dou.cbegin(), "hello");
  EXPECT_EQ(*result.first, dou.front());
  EXPECT_EQ(result.second, true);
  EXPECT_EQ(dou.deque(), (std::deque<std::string>{"hello", "world"}));
}

TEST(DequeOfUniqueTest, InsertDuplicateElement) {
  containerofunique::deque_of_unique<std::string> dou = {"hello", "world"};
  auto result = dou.insert(dou.cend(), "hello");
  EXPECT_EQ(result.first, dou.cend());
  EXPECT_EQ(result.second, false);
  EXPECT_EQ(dou.deque(), (std::deque<std::string>{"hello", "world"}));
}

TEST(DequeOfUniqueTest, InsertIntoEmptyDeque) {
  containerofunique::deque_of_unique<std::string> dou;
  auto result = dou.insert(dou.cend(), "first");
  EXPECT_EQ(*result.first, dou.front());
  EXPECT_EQ(result.second, true);
  EXPECT_EQ(dou.deque(), (std::deque<std::string>{"first"}));
}

TEST(DequeOfUniqueTest, InsertAtSpecificPosition) {
  containerofunique::deque_of_unique<std::string> dou = {"hello", "world"};
  auto it = dou.cbegin();
  auto result = dou.insert(it + 1, "goodbye");

  EXPECT_EQ(result.first, dou.cbegin() + 1);
  EXPECT_EQ(result.second, true);
  EXPECT_EQ(dou.deque(),
            (std::deque<std::string>{"hello", "goodbye", "world"}));
}

TEST(DequeOfUniqueTest, EmplaceIntoEmpty) {
  containerofunique::deque_of_unique<std::string> dou;
  std::deque<std::string> dq;

  auto result = dou.emplace(dou.cbegin(), "hello");
  dq.emplace(dq.begin(), "hello");

  EXPECT_EQ(dou.deque(), dq);
  EXPECT_EQ(*result.first, *dou.cbegin());
  EXPECT_TRUE(result.second);
}

TEST(DequeOfUniqueTest, EmplaceAtEnd) {
  containerofunique::deque_of_unique<std::string> dou = {"hello", "world"};
  std::deque<std::string> dq = {"hello", "world"};

  auto result = dou.emplace(dou.cend(), "goodbye");
  dq.emplace(dq.end(), "goodbye");

  EXPECT_EQ(dou.deque(), dq);
  EXPECT_EQ(*result.first, *(dou.cend() - 1));
  EXPECT_TRUE(result.second);
}

struct ThrowingType {
  std::string value;
  ThrowingType(const std::string &val) : value(val) {
    if (val == "throw") {
      throw std::runtime_error("Test exception");
    }
  }
  bool operator==(const ThrowingType &other) const {
    return value == other.value;
  }
};

namespace std {
template <>
struct hash<ThrowingType> {
  size_t operator()(const ThrowingType &obj) const {
    (void)obj;
    return 0;
  }
};
}  // namespace std

TEST(DequeOfUniqueTest, EmplaceExceptionSafety) {
  containerofunique::deque_of_unique<ThrowingType> dou;

  // Normal insertion
  EXPECT_NO_THROW(dou.emplace(dou.cbegin(), "hello"));

  // Exception-throwing insertion
  EXPECT_THROW(dou.emplace(dou.cbegin(), "throw"), std::runtime_error);

  // Ensure the container remains consistent
  EXPECT_EQ(dou.deque().size(), 1);
  EXPECT_EQ(dou.deque().front().value, "hello");
}

TEST(DequeOfUniqueTest, EmplaceNonString) {
  containerofunique::deque_of_unique<int> dou = {1, 2, 3};
  std::deque<int> dq = {1, 2, 3};

  auto result = dou.emplace(dou.cbegin(), 4);
  dq.emplace(dq.begin(), 4);

  EXPECT_EQ(dou.deque(), dq);
  EXPECT_EQ(*result.first, *dou.cbegin());
  EXPECT_TRUE(result.second);

  // Attempt to emplace a duplicate
  result = dou.emplace(dou.cbegin(), 4);
  EXPECT_EQ(dou.deque(), dq);  // No change
  EXPECT_FALSE(result.second);
}

TEST(DequeOfUniqueTest, EmplaceFront_NewElement) {
  // Test 1: Emplace a new element "good" to the front of dou
  // Expected: "good" should be at the front of the deque
  containerofunique::deque_of_unique<std::string> dou = {"hello", "world"};
  std::deque<std::string> dq = {"hello", "world"};
  auto result = dou.emplace_front("good");
  dq.emplace_front("good");
  EXPECT_EQ(*dou.cbegin(), "good");
  ASSERT_TRUE(result.has_value());
  // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
  EXPECT_EQ(result.value().get(), "good");
  EXPECT_EQ(dou.deque(), dq);
}

TEST(DequeOfUniqueTest, EmplaceFront_DuplicateElement) {
  // Test 2: Try emplacing "good" twice (duplicate value)
  // Expected: No insertion, deque remains unchanged
  containerofunique::deque_of_unique<std::string> dou = {"hello", "world"};
  std::deque<std::string> dq = {"hello", "world"};
  dou.emplace_front("good");
  dq.emplace_front("good");
  auto result = dou.emplace_front("good");
  EXPECT_EQ(*dou.cbegin(), "good");
  EXPECT_EQ(result, std::nullopt);
  EXPECT_EQ(dou.deque(), dq);
}

TEST(DequeOfUniqueTest, EmplaceFront_Rvalue) {
  // Test 3: Emplace an rvalue "good" to the front of dou
  // Expected: "good" should be inserted at the front of the deque
  containerofunique::deque_of_unique<std::string> dou = {"hello", "world"};
  std::string str = "good";
  auto result = dou.emplace_front(std::move(str));
  std::deque<std::string> dq = {"good", "hello", "world"};
  EXPECT_EQ(*dou.cbegin(), "good");
  ASSERT_TRUE(result.has_value());
  // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
  EXPECT_EQ(result.value().get(), "good");
  EXPECT_EQ(dou.deque(), dq);
}

TEST(DequeOfUniqueTest, EmplaceFront_DuplicateRvalue) {
  // Test 4: Try emplacing the rvalues "good" twice (duplicate value)
  // Expected: No insertion, deque remains unchanged
  containerofunique::deque_of_unique<std::string> dou = {"hello", "world"};
  std::string str1 = "good";
  std::string str2 = "good";
  dou.emplace_front(std::move(str1));
  auto result = dou.emplace_front(std::move(str2));
  std::deque<std::string> dq = {"good", "hello", "world"};
  EXPECT_EQ(*dou.cbegin(), "good");
  EXPECT_EQ(result, std::nullopt);
  EXPECT_EQ(dou.deque(), dq);
}

TEST(DequeOfUniqueTest, EmplaceFront_EmptyDeque) {
  // Test 5: Emplace a new element "first" to an empty deque
  // Expected: "first" should be at the front of the deque
  containerofunique::deque_of_unique<std::string> dou_empty;
  auto result_empty = dou_empty.emplace_front("first");
  std::deque<std::string> dq = {"first"};
  EXPECT_EQ(*dou_empty.cbegin(), "first");
  ASSERT_TRUE(result_empty.has_value());
  // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
  EXPECT_EQ(result_empty.value().get(), "first");
  EXPECT_EQ(dou_empty.deque(), dq);
}

TEST(DequeOfUniqueTest, EmplaceFront_MultipleElements) {
  // Test 6: Emplace multiple distinct elements to the front of the deque
  // Expected: Elements should be inserted at the front in the order of
  // emplace_front calls
  containerofunique::deque_of_unique<std::string> dou = {"hello"};
  dou.emplace_front("world");
  dou.emplace_front("good");
  dou.emplace_front("morning");

  std::deque<std::string> dq = {"morning", "good", "world", "hello"};

  EXPECT_EQ(*dou.cbegin(), "morning");
  EXPECT_EQ(*(dou.cbegin() + 1), "good");
  EXPECT_EQ(*(dou.cbegin() + 2), "world");
  EXPECT_EQ(*(dou.cbegin() + 3), "hello");
  EXPECT_EQ(dou.deque(), dq);
}

TEST(DequeOfUniqueTest, EmplaceFront_NonStringType) {
  // Test 7: Emplace an integer to the front of an integer deque (non-string
  // type) Expected: The integer 4 should be at the front of the deque
  containerofunique::deque_of_unique<int> dou = {1, 2, 3};
  auto result = dou.emplace_front(4);
  std::deque<int> dq = {1, 2, 3, 4};
  EXPECT_EQ(*dou.cbegin(), 4);
  ASSERT_TRUE(result.has_value());
  // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
  EXPECT_EQ(result.value().get(), 4);
}

TEST(DequeOfUniqueTest, EmplaceBack_NewElement) {
  // Test 1: Emplace a new element "good" to the end of dou
  // Emplace_back "good" to dou1
  containerofunique::deque_of_unique<std::string> dou = {"hello", "world"};
  std::deque<std::string> dq = {"hello", "world"};
  auto result = dou.emplace_back("good");
  dq.emplace_back("good");
  EXPECT_EQ(*(dou.cend() - 1), "good");
  ASSERT_TRUE(result.has_value());
  // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
  EXPECT_EQ(result.value().get(), "good");
  EXPECT_EQ(dou.deque(), dq);
}

TEST(DequeOfUniqueTest, EmplaceBack_DuplicateElement) {
  // Test 2: Try emplacing "good" twice (duplicate value)
  // Expected: No insertion, deque remains unchanged
  containerofunique::deque_of_unique<std::string> dou = {"hello", "world"};
  std::deque<std::string> dq = {"hello", "world"};
  dou.emplace_back("good");
  dq.emplace_back("good");
  auto result = dou.emplace_back("good");
  EXPECT_EQ(*(dou.cend() - 1), "good");
  EXPECT_EQ(result, std::nullopt);
  EXPECT_EQ(dou.deque(), dq);
}

TEST(DequeOfUniqueTest, EmplaceBack_Rvalue) {
  // Test 3: Emplace an rvalue "good" to the back of dou
  // Expected: "good" should be inserted at the end of the deque
  containerofunique::deque_of_unique<std::string> dou = {"hello", "world"};
  std::string str = "good";
  auto result = dou.emplace_back(std::move(str));
  std::deque<std::string> dq = {"hello", "world", "good"};
  EXPECT_EQ(*(dou.cend() - 1), "good");
  ASSERT_TRUE(result.has_value());
  // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
  EXPECT_EQ(result.value().get(), "good");
  EXPECT_EQ(dou.deque(), dq);
}

TEST(DequeOfUniqueTest, EmplaceBack_DuplicateRvalue) {
  // Test 4: Try emplacing the rvalues "good" twice (duplicate value)
  // Expected: No insertion, deque remains unchanged
  containerofunique::deque_of_unique<std::string> dou = {"hello", "world"};
  std::string str1 = "good";
  std::string str2 = "good";
  dou.emplace_back(std::move(str1));
  auto result = dou.emplace_back(std::move(str2));
  std::deque<std::string> dq = {"hello", "world", "good"};
  EXPECT_EQ(*(dou.cend() - 1), "good");
  EXPECT_EQ(result, std::nullopt);
  EXPECT_EQ(dou.deque(), dq);
}

TEST(DequeOfUniqueTest, EmplaceBack_EmptyDeque) {
  // Test 5: Emplace a new element "first" to an empty deque
  // Expected: "first" should be at the back of the deque
  containerofunique::deque_of_unique<std::string> dou_empty;
  auto result_empty = dou_empty.emplace_back("first");
  std::deque<std::string> dq = {"first"};
  EXPECT_EQ(*dou_empty.cbegin(), "first");
  ASSERT_TRUE(result_empty.has_value());
  // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
  EXPECT_EQ(result_empty.value().get(), "first");
  EXPECT_EQ(dou_empty.deque(), dq);
}

TEST(DequeOfUniqueTest, EmplaceBack_MultipleElements) {
  // Test 6: Emplace multiple distinct elements to the back of the deque
  // Expected: Elements should be inserted at the back in the order of
  // emplace_back calls
  containerofunique::deque_of_unique<std::string> dou = {"hello"};
  dou.emplace_back("world");
  dou.emplace_back("good");
  dou.emplace_back("morning");

  std::deque<std::string> dq = {"hello", "world", "good", "morning"};

  EXPECT_EQ(*(dou.cend() - 1), "morning");
  EXPECT_EQ(*(dou.cend() - 2), "good");
  EXPECT_EQ(*(dou.cend() - 3), "world");
  EXPECT_EQ(*(dou.cend() - 4), "hello");
  EXPECT_EQ(dou.deque(), dq);
}

TEST(DequeOfUniqueTest, EmplaceBack_NonStringType) {
  // Test 7: Emplace an integer to the back of an integer deque (non-string
  // type) Expected: The integer 4 should be at the back of the deque
  containerofunique::deque_of_unique<int> dou = {1, 2, 3};
  auto result = dou.emplace_back(4);
  std::deque<int> dq = {1, 2, 3, 4};
  EXPECT_EQ(*(dou.cend() - 1), 4);
  ASSERT_TRUE(result.has_value());
  // NOLINTNEXTLINE(bugprone-unchecked-optional-access,-warnings-as-errors)
  EXPECT_EQ(result.value().get(), 4);
  EXPECT_EQ(dou.deque(), dq);
}

TEST(DequeOfUniqueTest, PopFront_EmptyDeque) {
  containerofunique::deque_of_unique<std::string> dou;
  EXPECT_NO_THROW(dou.pop_front());
  EXPECT_TRUE(dou.deque().empty());
  EXPECT_TRUE(dou.set().empty());
}

TEST(DequeOfUniqueTest, PopFront_SingleElement) {
  containerofunique::deque_of_unique<std::string> dou = {"hello"};
  dou.pop_front();
  EXPECT_TRUE(dou.deque().empty());
  EXPECT_TRUE(dou.set().empty());
}

TEST(DequeOfUniqueTest, PopFront_MultipleSequential) {
  containerofunique::deque_of_unique<std::string> dou = {"hello", "world",
                                                         "goodbye"};
  dou.pop_front();
  EXPECT_EQ(dou.deque(), (std::deque<std::string>{"world", "goodbye"}));
  EXPECT_THAT(dou.set(), ::testing::UnorderedElementsAre("world", "goodbye"));

  dou.pop_front();
  EXPECT_EQ(dou.deque(), (std::deque<std::string>{"goodbye"}));
  EXPECT_THAT(dou.set(), ::testing::UnorderedElementsAre("goodbye"));

  dou.pop_front();
  EXPECT_TRUE(dou.deque().empty());
  EXPECT_TRUE(dou.set().empty());
}

TEST(DequeOfUniqueTest, Front_AfterModification) {
  containerofunique::deque_of_unique<std::string> dou = {"hello", "world"};

  // Add a new element at the front
  dou.emplace_front("good");
  EXPECT_EQ(dou.front(), "good");  // The front should now be "good"

  // Remove the front element
  dou.pop_front();
  EXPECT_EQ(dou.front(), "hello");  // The front should now be "hello"
}

TEST(DequeOfUniqueTest, PopBack_EmptyDeque) {
  containerofunique::deque_of_unique<std::string> dou;
  EXPECT_NO_THROW(dou.pop_back());
  EXPECT_TRUE(dou.deque().empty());
  EXPECT_TRUE(dou.set().empty());
}

TEST(DequeOfUniqueTest, PopBack_SingleElement) {
  containerofunique::deque_of_unique<std::string> dou = {"hello"};
  dou.pop_back();
  EXPECT_TRUE(dou.deque().empty());
  EXPECT_TRUE(dou.set().empty());
}

TEST(DequeOfUniqueTest, PopBack_MultipleSequential) {
  containerofunique::deque_of_unique<std::string> dou = {"hello", "world",
                                                         "goodbye"};
  dou.pop_back();
  EXPECT_EQ(dou.deque(), (std::deque<std::string>{"hello", "world"}));
  EXPECT_THAT(dou.set(), ::testing::UnorderedElementsAre("hello", "world"));

  dou.pop_back();
  EXPECT_EQ(dou.deque(), (std::deque<std::string>{"hello"}));
  EXPECT_THAT(dou.set(), ::testing::UnorderedElementsAre("hello"));

  dou.pop_back();
  EXPECT_TRUE(dou.deque().empty());
  EXPECT_TRUE(dou.set().empty());
}

TEST(DequeOfUniqueTest, PushFront_NewElement) {
  containerofunique::deque_of_unique<std::string> dou = {"hello", "world"};
  std::deque<std::string> expected = {"good", "hello", "world"};

  // Test pushing a new element to the front
  bool result = dou.push_front("good");
  EXPECT_TRUE(result);
  EXPECT_EQ(dou.deque(), expected);
  EXPECT_THAT(dou.set(), ::testing::UnorderedElementsAreArray(expected));
}

TEST(DequeOfUniqueTest, PushFront_DuplicateElement) {
  containerofunique::deque_of_unique<std::string> dou = {"hello", "world"};
  std::deque<std::string> expected = {"hello", "world"};

  // Test pushing a duplicate element
  bool result = dou.push_front("hello");
  EXPECT_FALSE(result);
  EXPECT_EQ(dou.deque(), expected);
  EXPECT_THAT(dou.set(), ::testing::UnorderedElementsAreArray(expected));
}

TEST(DequeOfUniqueTest, PushFront_Rvalue) {
  containerofunique::deque_of_unique<std::string> dou = {"hello", "world"};
  std::deque<std::string> expected = {"good", "hello", "world"};

  // Test pushing an rvalue to the front
  std::string str = "good";
  bool result = dou.push_front(std::move(str));
  EXPECT_TRUE(result);
  EXPECT_EQ(dou.deque(), expected);
  EXPECT_THAT(dou.set(), ::testing::UnorderedElementsAreArray(expected));
}

TEST(DequeOfUniqueTest, PushFront_EmptyRvalue) {
  containerofunique::deque_of_unique<std::string> dou = {"hello", "world"};
  std::deque<std::string> expected = {"", "hello", "world"};

  // Test pushing an empty string as an rvalue
  std::string str = "";
  bool result = dou.push_front(std::move(str));
  EXPECT_TRUE(result);
  EXPECT_EQ(dou.deque(), expected);
  EXPECT_THAT(dou.set(), ::testing::UnorderedElementsAreArray(expected));
}

TEST(DequeOfUniqueTest, PushBack_NewElement) {
  containerofunique::deque_of_unique<std::string> dou = {"hello", "world"};
  std::deque<std::string> expected = {"hello", "world", "good"};

  // Test pushing a new element to the back
  bool result = dou.push_back("good");
  EXPECT_TRUE(result);  // Should return true
  EXPECT_EQ(dou.deque(), expected);
  EXPECT_THAT(dou.set(), ::testing::UnorderedElementsAreArray(expected));
}

TEST(DequeOfUniqueTest, PushBack_DuplicateElement) {
  containerofunique::deque_of_unique<std::string> dou = {"hello", "world"};
  std::deque<std::string> expected = {"hello", "world"};

  // Test pushing a duplicate element
  bool result = dou.push_back("hello");
  EXPECT_FALSE(result);  // Should return false
  EXPECT_EQ(dou.size(), 2);
  EXPECT_EQ(dou.deque(), expected);
  EXPECT_THAT(dou.set(), ::testing::UnorderedElementsAreArray(expected));
}

TEST(DequeOfUniqueTest, PushBack_Rvalue) {
  containerofunique::deque_of_unique<std::string> dou = {"hello", "world"};
  std::deque<std::string> expected = {"hello", "world", "good"};

  // Test pushing an rvalue to the back
  std::string str = "good";
  bool result = dou.push_back(std::move(str));
  EXPECT_TRUE(result);  // Should return true
  EXPECT_EQ(dou.deque(), expected);
  EXPECT_THAT(dou.set(), ::testing::UnorderedElementsAreArray(expected));
}

TEST(DequeOfUniqueTest, PushBack_EmptyRvalue) {
  containerofunique::deque_of_unique<std::string> dou = {"hello", "world"};
  std::deque<std::string> expected = {"hello", "world", ""};

  // Test pushing an empty string as an rvalue
  std::string str = "";
  bool result = dou.push_back(std::move(str));
  EXPECT_TRUE(result);  // Should return true
  EXPECT_EQ(dou.deque(), expected);
  EXPECT_THAT(dou.set(), ::testing::UnorderedElementsAreArray(expected));
}

TEST(DequeOfUniqueTest, PushBack_EmptyContainer) {
  containerofunique::deque_of_unique<std::string> dou;
  std::deque<std::string> expected = {"hello"};

  // Test pushing to an initially empty container
  bool result = dou.push_back("hello");
  EXPECT_TRUE(result);  // Should return true
  EXPECT_EQ(dou.deque(), expected);
  EXPECT_THAT(dou.set(), ::testing::UnorderedElementsAreArray(expected));
}

TEST(DequeOfUniqueTest, MemberFunctionSwap) {
  containerofunique::deque_of_unique<std::string> dou1 = {"hello", "world"};
  containerofunique::deque_of_unique<std::string> dou2 = {"good", "morning"};
  std::deque<std::string> dq1 = {"hello", "world"};
  std::deque<std::string> dq2 = {"good", "morning"};

  dou1.swap(dou2);
  EXPECT_EQ(dou1.deque(), dq2);
  EXPECT_THAT(dou1.set(), ::testing::UnorderedElementsAreArray(dq2));
  EXPECT_EQ(dou2.deque(), dq1);
  EXPECT_THAT(dou2.set(), ::testing::UnorderedElementsAreArray(dq1));
}

TEST(DequeOfUniqueTest, StdSwap) {
  containerofunique::deque_of_unique<std::string> dou1 = {"hello", "world"};
  containerofunique::deque_of_unique<std::string> dou2 = {"good", "morning"};
  std::deque<std::string> dq1 = {"hello", "world"};
  std::deque<std::string> dq2 = {"good", "morning"};

  std::swap(dou1, dou2);
  EXPECT_EQ(dou1.deque(), dq2);
  EXPECT_THAT(dou1.set(), ::testing::UnorderedElementsAreArray(dq2));
  EXPECT_EQ(dou2.deque(), dq1);
  EXPECT_THAT(dou2.set(), ::testing::UnorderedElementsAreArray(dq1));
}

TEST(DequeOfUniqueTest, Empty) {
  containerofunique::deque_of_unique<std::string> dou1;
  EXPECT_TRUE(dou1.empty());
  containerofunique::deque_of_unique<std::string> dou2 = {"good"};
  EXPECT_FALSE(dou2.empty());
  containerofunique::deque_of_unique<std::string> dou3 = {"good", "morning",
                                                          "hello", "world"};
  EXPECT_FALSE(dou3.empty());
}

TEST(DequeOfUniqueTest, Size) {
  // Test 1: Deque with a single element
  containerofunique::deque_of_unique<std::string> dou1 = {"good"};
  EXPECT_EQ(dou1.size(), 1);

  // Test 2: Deque with multiple unique elements
  containerofunique::deque_of_unique<std::string> dou2 = {"good", "morning",
                                                          "hello", "world"};
  EXPECT_EQ(dou2.size(), 4);

  // Adding a new unique element increases the size
  dou2.push_back("new");
  EXPECT_EQ(dou2.size(), 5);

  // Removing an element decreases the size
  dou2.pop_front();
  EXPECT_EQ(dou2.size(), 4);

  // Attempting to add a duplicate element does not change the size
  dou2.push_back("morning");  // "morning" is already in the deque
  EXPECT_EQ(dou2.size(), 4);

  // Test 3: Empty deque
  containerofunique::deque_of_unique<std::string> dou3;
  EXPECT_EQ(dou3.size(), 0);  // Corrected to check dou3
}

TEST(DequeOfUniqueTest, Operator) {
  // Test 1: Identical deques
  containerofunique::deque_of_unique<std::string> dou1_1 = {"good"};
  containerofunique::deque_of_unique<std::string> dou1_2 = {"good"};
  EXPECT_EQ(dou1_1 <=> dou1_2, std::strong_ordering::equal);

  // Test 2: Subset case
  containerofunique::deque_of_unique<std::string> dou2 = {"good", "morning"};
  EXPECT_EQ(dou1_1 <=> dou2, std::weak_ordering::less);
  EXPECT_EQ(dou2 <=> dou1_1, std::weak_ordering::greater);

  // Test 3: Different order case (for robustness, even if not expected in
  // unique deques)
  containerofunique::deque_of_unique<std::string> dou3 = {"morning", "good"};
  EXPECT_EQ(dou2 <=> dou3, std::weak_ordering::less);

  // Test 4: Lexicographical comparison
  containerofunique::deque_of_unique<std::string> dou4 = {"apple", "banana"};
  EXPECT_EQ(dou4 <=> dou1_1, std::weak_ordering::less);

  // Test 5: Empty deques
  containerofunique::deque_of_unique<std::string> dou_empty1;
  containerofunique::deque_of_unique<std::string> dou_empty2;
  EXPECT_EQ(dou_empty1 <=> dou_empty2, std::strong_ordering::equal);
  EXPECT_EQ(dou_empty1 <=> dou1_1, std::weak_ordering::less);
}
