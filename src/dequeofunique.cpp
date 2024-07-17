// #pragma once

#include <deque>
#include <functional>  // For std::hash
#include <initializer_list>
#include <iostream>
#include <memory>       // For std::allocator
#include <ranges>       // For ranges
#include <type_traits>  // For std::is_same
#include <unordered_set>

namespace containerofunique {

template <class T, class Hash = std::hash<T>, class KeyEqual = std::equal_to<T>,
          class Allocator = std::allocator<T> >
// To do list 3: is it necessary to have _Pred and _Alloc?
// Do not use _Pred and _Alloc for now and only focus on dequeofuniqueints for
// now
class dequeofunique {
  std::deque<T, Allocator> deque_;
  std::unordered_set<T, Hash, KeyEqual, Allocator> set_;

 public:
  // *Member types
  using value_type = T;
  using key_type = T;
  using hasher = Hash;
  using key_equal = KeyEqual;
  using allocator_type = Allocator;

  static_assert(
      std::is_same<typename allocator_type::value_type, value_type>::value,
      "Allocator::value_type must be same type as value_type");

  using __alloc_traits = std::allocator_traits<Allocator>;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = typename __alloc_traits::pointer;
  using size_type = typename __alloc_traits::size_type;
  using difference_type = typename __alloc_traits::difference_type;
  using _deque_iterator = typename std::deque<T, Allocator>::iterator;
  using _deque_const_iterator =
      typename std::deque<T, Allocator>::const_iterator;
  using _unordered_set_iterator =
      typename std::unordered_set<T, Hash, KeyEqual, Allocator>::iterator;
  using _unordered_set_const_iterator =
      typename std::unordered_set<T, Hash, KeyEqual, Allocator>::const_iterator;
  using _deque_reverse_iterator =
      typename std::deque<T, Allocator>::reverse_iterator;
  using _deque_const_reverse_iterator =
      typename std::deque<T, Allocator>::const_reverse_iterator;

  using iterator = _deque_iterator;
  using const_iterator = _deque_const_iterator;
  using reverse_iterator = _deque_reverse_iterator;
  using const_reverse_iterator = _deque_const_reverse_iterator;

  // *Member functions
  dequeofunique() = default;

  // To do list 5:
  explicit dequeofunique(const Allocator& alloc);  // Can be done later
  // End of to do list 5:

  // To do list 1:
  template <class InputIt>
  dequeofunique(InputIt first, InputIt last,
                const Allocator& alloc = Allocator()) {
    push_back(first, last);
  }

  dequeofunique(const std::initializer_list<T>& init,
                const Allocator& alloc = Allocator())
      : dequeofunique(init.begin(), init.end()) {}

  // End of to do list 1:

  dequeofunique(const dequeofunique& other) = default;

  // To do list 2:
  dequeofunique(const dequeofunique& other, const Allocator& alloc);

  dequeofunique(dequeofunique&& other) {
    std::swap(deque_, other.deque_);
    std::swap(set_, other.set_);
  };

  dequeofunique(dequeofunique&& other, const Allocator& alloc);

  // template <std::ranges::input_range R>
  // dequeofunique(std::from_range_t, R&& rg,
  //               const Allocator& alloc = Allocator());
  //  End of to do list 2:

  dequeofunique& operator=(const dequeofunique& other) = default;
  dequeofunique& operator=(dequeofunique&& other) noexcept {
    std::swap(deque_, other.deque_);
    std::swap(set_, other.set_);
    return *this;
  };
  dequeofunique& operator=(std::initializer_list<T> ilist) {
    dequeofunique temp(ilist);
    std::swap(deque_, ilist.deque_);
    std::swap(set_, ilist.set_);
    return *this;
  };

  _deque_iterator __deque_begin() { return deque_.begin(); }
  _deque_iterator __deque_end() { return deque_.end(); }
  _deque_const_iterator __deque_cbegin() const { return deque_.begin(); }
  _deque_const_iterator __deque_cend() const { return deque_.end(); }

  _deque_iterator __deque_rbegin() { return deque_.rbegin(); }
  _deque_iterator __deque_rend() { return deque_.rend(); }
  _deque_const_iterator __deque_crbegin() const { return deque_.rbegin(); }
  _deque_const_iterator __deque_crend() const { return deque_.rend(); }

  _unordered_set_iterator __set_begin() { return set_.begin(); }
  _unordered_set_iterator __set_end() { return set_.end(); }
  _unordered_set_const_iterator __set_begin() const { return set_.begin(); }
  _unordered_set_const_iterator __set_end() const { return set_.end(); }

  iterator begin() { return __deque_begin(); }
  iterator end() { return __deque_end(); }
  const_iterator cbegin() const { return __deque_cbegin(); }
  const_iterator cend() const { return __deque_cend(); }

  iterator rbegin() { return __deque_rbegin(); }
  iterator rend() { return __deque_rend(); }
  const_iterator crbegin() const { return __deque_crbegin(); }
  const_iterator crend() const { return __deque_crend(); }

  template <class InputIt>
  void push_back(InputIt first, InputIt last) {
    while (first != last) {
      push_back(*first++);
    }
  }

  bool push_back(T entry) {
    if (set_.emplace(entry).second) {
      deque_.push_back(entry);
      return true;
    }
    return false;
  }

  bool push_back(const dequeofunique<T, Hash>& other) {
    return push_back(other.deque_());
  }

  bool push_back(const std::deque<T>& other) {
    bool any_added = false;
    for (const auto& entry : other) {
      auto added = push_back(entry);
      any_added = any_added || added;
    }
    return any_added;
  }

  std::deque<T, Allocator> deque() { return deque_; }

  std::unordered_set<T, Hash, KeyEqual, Allocator> set() { return set_; }

  void print() const {
    std::cout << "Deque: ";
    for (const auto& elem : deque_) {
      std::cout << elem << " ";
    }
    std::cout << ".\n";
    std::cout << "Size of deque is: " << deque_.size() << ".\n";

    std::cout << "Set: ";
    for (const auto& elem : set_) {
      std::cout << elem << " ";
    }
    std::cout << "\n";
    std::cout << "Size of set is: " << set_.size() << ".\n";
  }
};
}  // namespace containerofunique

void print(std::deque<T> dq) {
  for (T entry : dq) {
    std::cout <<
  }
}

void print_set() {}

int main() {
  std::cout << "Test constructors using int:\n";
  //  To do list 6: print out the objects below:
  containerofunique::dequeofunique<int> dq_int_empty;
  containerofunique::dequeofunique<int> dq_int_init1{1};
  containerofunique::dequeofunique<int> dq_int_init2{1, 2};
  containerofunique::dequeofunique<int> dq_int_init3{1, 2, 2, 2};
  containerofunique::dequeofunique<int> dq_int_init4(dq_int_init3);
  containerofunique::dequeofunique<int> dq_int_init5(std::move(dq_int_init3));
  containerofunique::dequeofunique<int> dq_int_init6 = dq_int_init3;
  containerofunique::dequeofunique<int> dq_int_init7 = std::move(dq_int_init3);
  containerofunique::dequeofunique<int> dq_int_init8 = {1, 2, 2, 2};
  // End of to do list 6

  std::cout << "Test push_back():\n";
  // To do list 7: print out the objects after push_back;
  containerofunique::dequeofunique<int> dq_int_init1.push_back(1);
  containerofunique::dequeofunique<int> dq_int_init1.push_back(2);
  containerofunique::dequeofunique<int> dq_int_init1.push_back(2, 2);
  // To do list 7:
}
