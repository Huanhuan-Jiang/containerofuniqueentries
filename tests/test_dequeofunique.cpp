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

  EXPECT_TRUE(std::is_const<typename std::remove_reference<decltype(*dou.cbegin())>::type>::value);
  EXPECT_TRUE(std::is_const<typename std::remove_reference<decltype(*dou.cend())>::type>::value);
  EXPECT_TRUE(std::is_const<typename std::remove_reference<decltype(*dou.crbegin())>::type>::value);
  EXPECT_TRUE(std::is_const<typename std::remove_reference<decltype(*dou.crend())>::type>::value);
}
