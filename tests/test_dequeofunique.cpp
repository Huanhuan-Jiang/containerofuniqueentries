#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <deque>
#include <stdexcept>
#include <unordered_set>

#include "dequeofunique.h"

using testing::HasSubstr;
using testing::Property;
using testing::Throws;

TEST(DequeOfUniqueTest, ReturnDequeAndSet) {
  std::deque<std::pair<int, int>> deque1 = {
      {20, 30}, {19, 30}, {19, 29}, {18, 28}};
  std::deque<std::pair<int, int>> deque2 = {
      {19, 29}, {19, 30}, {19, 31}, {18, 29}};

  std::unordered_set<std::pair<int, int>, gamestatus::pair_hash> set1 = {
      {20, 30}, {19, 30}, {19, 29}, {18, 28}};
  std::unordered_set<std::pair<int, int>, gamestatus::pair_hash> set2 = {
      {19, 29}, {19, 30}, {19, 31}, {18, 29}};

  gamestatus::DequeOfUniquePairs<int, int> deque_unique1(deque1);
  gamestatus::DequeOfUniquePairs<int, int> deque_unique2(deque2);
  EXPECT_EQ(deque_unique1.deque(), deque1);
  EXPECT_EQ(deque_unique2.deque(), deque2);
  EXPECT_EQ(deque_unique1.set(), set1);
  EXPECT_EQ(deque_unique2.set(), set2);
}

