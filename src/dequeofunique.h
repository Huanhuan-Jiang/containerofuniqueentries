#pragma once

#include <deque>
#include <initializer_list>
#include <unordered_set>

namespace containerofunique {
template <class _Value, class _Hash = std::hash<_Value>, class _Pred = std::equal_to<_Value>, class _Alloc = std::allocator<_Value> >
class dequeofunique {
  std::deque<T> deque_;
  std::unordered_set<T, Hash> set_;

 public:
  using value_type = _Value;
  using allocator_type = _Alloc;
  using __alloc_traits = allocator_traits<allocator_type>;
  static_assert(__check_valid_allocator<allocator_type>::value, "");
  static_assert(is_same<typename allocator_type::value_type, value_type>::value,
              "Allocator::value_type must be same type as value_type");



  dequeofunique() = default;

  dequeofunique(const std::initializer_list<T>& initializer)
      : dequeofunique(initializer.begin(), initializer.end()) {}

  dequeofunique(const dequeofunique& other) = default;

  dequeofunique& operator=(dequeofunique& other) = default;

  template <class InputIt>
  dequeofunique(InputIt first, InputIt last) {
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

  bool pushBack(const dequeofunique<T, Hash>& other) {
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