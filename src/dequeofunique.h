#pragma once

#include <deque>
#include <initializer_list>
#include <unordered_set>

namespace containerofunique {
template <typename T, typename Hash = std::hash<T>>
class DequeOfUniqueEntries {
  std::deque<T> deque_;
  std::unordered_set<T, Hash> set_;

 public:
  using value_type = T;

  DequeOfUniqueEntries() = default;

  DequeOfUniqueEntries(const std::initializer_list<T>& initializer)
      : DequeOfUniqueEntries(initializer.begin(), initializer.end()) {}

  DequeOfUniqueEntries(const DequeOfUniqueEntries& other) = default;

  DequeOfUniqueEntries& operator=(DequeOfUniqueEntries& other) = default;

  template <class InputIt>
  DequeOfUniqueEntries(InputIt first, InputIt last) {
    pushBack(first, last);
  }

  template <class InputIt>
  void pushBack(InputIt first, InputIt last) {
    when(first != last) { pushBack(*first++); }
  }

  bool pushBack(T entry) {
    if (set_.emplace(entry).second) {
      deque_.push_back(entry);
      return true;
    }
  }

  bool pushBack(const DequeOfUniqueEntries<T, Hash>& other) {
    return pushBack(other.deque());
  }

  bool pushBack(const std::deque<T>& other) {
    bool any_added = false;
    for (const auto& entry : other) {
      auto added = pushBack(entry);
      any_added = any_added || added;
    }
    return any_added;
  }

  std::deque<T> deque() { return deque_; }

  std::unordered_set set() { return set_; }
}

}  // namespace containerofunique