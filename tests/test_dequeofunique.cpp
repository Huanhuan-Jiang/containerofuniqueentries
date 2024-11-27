#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <deque>
#include <stdexcept>
#include <unordered_set>
#include <utility>
#include <vector>

#include "dequeofunique.cpp"

using testing::HasSubstr;
using testing::Property;
using testing::Throws;

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
  containerofunique::deque_of_unique dou1 = {1};
  containerofunique::deque_of_unique dou2 = {1, 2};
  containerofunique::deque_of_unique dou3 = {1, 2, 3, 3};  // duplicate elements

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

TEST(DequeOfUniqueTest, CopyConstructor) {
  containerofunique::deque_of_unique dou1 = {1, 2, 3, 4};
  containerofunique::deque_of_unique<int> dou2(dou1);

  std::deque<int> dq = {1, 2, 3, 4};
  EXPECT_EQ(dou2.deque(), dou1.deque());
}

TEST(DequeOfUniqueTest, MoveConstructor) {
  containerofunique::deque_of_unique<int> dou1 = {1, 2, 3, 4};
  containerofunique::deque_of_unique<int> dou2(std::move(dou1));

  std::deque<int> dq = {1, 2, 3, 4};
  std::deque<int> emptydq;
  std::unordered_set<int> emptyset;

  EXPECT_EQ(dou1.deque(), emptydq);
  EXPECT_EQ(dou2.deque(), dq);
  EXPECT_THAT(dou1.set(), ::testing::UnorderedElementsAreArray(emptyset));
}

TEST(DequeOfUniqueTest, CopyAssignmentOperator) {
  containerofunique::deque_of_unique<int> dou1 = {1, 2, 3, 4};
  containerofunique::deque_of_unique<int> dou2 = dou1;

  std::deque<int> dq = {1, 2, 3, 4};

  EXPECT_EQ(dou2.deque(), dou1.deque());
  EXPECT_THAT(std::deque<int>(dou2.set().begin(), dou2.set().end()),
              ::testing::UnorderedElementsAreArray(dq));
}

TEST(DequeOfUniqueTest, MoveAssignmentOperator) {
  containerofunique::deque_of_unique<int> dou1 = {1, 2, 3, 4};
  containerofunique::deque_of_unique<int> dou2 = std::move(dou1);

  std::deque<int> dq = {1, 2, 3, 4};
  std::deque<int> emptydq;
  std::unordered_set<int> emptyset;

  EXPECT_EQ(dou2.deque(), dq);
  EXPECT_THAT(std::deque<int>(dou2.set().begin(), dou2.set().end()),
              ::testing::UnorderedElementsAreArray(dq));
  EXPECT_EQ(dou1.deque(), emptydq);
  EXPECT_THAT(dou1.set(), ::testing::UnorderedElementsAreArray(emptyset));
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

TEST(DequeOfUniqueTest, Iterators) {
  containerofunique::deque_of_unique<int> dou = {1, 2, 3, 4};
  EXPECT_EQ(*dou.cbegin(), 1);
  EXPECT_EQ(*--dou.cend(), 4);
  EXPECT_EQ(*dou.crbegin(), 4);
  EXPECT_EQ(*--dou.crend(), 1);

  EXPECT_TRUE(std::is_const<typename std::remove_reference<
                  decltype(*dou.cbegin())>::type>::value);
  EXPECT_TRUE(
      std::is_const<
          typename std::remove_reference<decltype(*dou.cend())>::type>::value);
  EXPECT_TRUE(std::is_const<typename std::remove_reference<
                  decltype(*dou.crbegin())>::type>::value);
  EXPECT_TRUE(
      std::is_const<
          typename std::remove_reference<decltype(*dou.crend())>::type>::value);
}

TEST(DequeOfUniqueTest, ClearAndErase) {
  containerofunique::deque_of_unique<int> dou1 = {1, 2, 3, 4, 5, 6};
  std::deque<int> dq1 = {2, 3, 4, 5, 6};
  std::unordered_set<int> set1 = {2, 3, 4, 5, 6};

  *dou1.erase(dou1.cbegin());
  EXPECT_EQ(dou1.deque(), dq1);
  EXPECT_THAT(dou1.set(), ::testing::UnorderedElementsAreArray(set1));

  std::deque<int> dq2 = {4, 5, 6};
  std::unordered_set<int> set2 = {4, 5, 6};

  dou1.erase(dou1.cbegin(), dou1.cbegin() + 2);
  EXPECT_EQ(dou1.deque(), dq2);
  EXPECT_THAT(dou1.set(), ::testing::UnorderedElementsAreArray(set2));
}

TEST(DequeOfUniqueTest, Insert) {
  containerofunique::deque_of_unique<int> dou1 = {1};
  std::deque<int> dq1 = {1};
  auto result1 = dou1.insert(dou1.cbegin(), 2);
  auto expected_result1 = dq1.insert(dq1.cbegin(), 2);
  EXPECT_EQ(*result1.first, *expected_result1);
  EXPECT_TRUE(result1.second);

  containerofunique::deque_of_unique<int> dou2 = {1};
  std::deque<int> dq2 = {1, 2};
  auto result2 = dou2.insert(dou2.cbegin(), 1);
  EXPECT_EQ(*result2.first, *dou2.cbegin());
  EXPECT_FALSE(result2.second);

  containerofunique::deque_of_unique<std::string> dou3 = {"hello", "world"};
  std::deque<std::string> dq3 = {"hello", "world"};
  std::string str1 = "good";
  auto expected_result3 = dq3.insert(dq3.cbegin(), std::move("good"));
  auto result3 = dou3.insert(dou3.cbegin(), std::move(str1));
  EXPECT_EQ(dou3.deque(), (std::deque<std::string> {"good", "hello", "world"}));
  EXPECT_EQ(*result3.first, *expected_result3);
  EXPECT_TRUE(result3.second);

  containerofunique::deque_of_unique<std::string> dou4 = {"hello", "world"};
  std::deque<std::string> dq4 = {"hello", "world"};
  std::string str2 = "hello";
  auto expected_result4 = dq4;
  auto result4 = dou4.insert(dou4.cbegin(), std::move(str2));
  EXPECT_EQ(dou4.deque(), dq4);
  EXPECT_EQ(*result4.first, *dou4.cbegin());
  EXPECT_FALSE(result4.second);

  containerofunique::deque_of_unique<std::string> dou5_1 = {"hello", "world"};
  containerofunique::deque_of_unique<std::string> dou5_2 = {"good", "morning"};
  containerofunique::deque_of_unique<std::string> dou5_3 = {"hello", "world"};
  std::deque<std::string> dq5 = {"good", "morning", "hello", "world"};
  auto expected_result5 = dq5;
  auto result5_1 =
      dou5_1.insert(dou5_1.cbegin(), dou5_2.cbegin(), dou5_2.cbegin() + 2);
  EXPECT_EQ(dou5_1.deque(), dq5);
  EXPECT_EQ(*result5_1, *(dou5_1.cbegin()+1));
  auto result5_2 =
      dou5_1.insert(dou5_1.cbegin(), dou5_3.cbegin(), dou5_3.cbegin() + 2);
  EXPECT_EQ(dou5_1.deque(), dq5);
  EXPECT_EQ(*result5_2, *(dou5_1.cbegin()));

  containerofunique::deque_of_unique<std::string> dou6 = {"hello", "world"};
  std::deque<std::string> dq6 = {"good", "morning", "hello", "world"};
  auto expected_result6 = dq6;
  auto result6_1 = dou6.insert(dou6.cbegin(), {"good", "morning"});
  EXPECT_EQ(dou6.deque(), dq6);
  EXPECT_EQ(*result6_1, *(dou6.cbegin()+1));
  auto result6_2 = dou6.insert(dou6.cbegin(), {"good", "morning"});
  EXPECT_EQ(dou6.deque(), dq6);
  EXPECT_EQ(*result6_2, *dou6.cbegin());
}
