// #pragma once

#include <algorithm>  // For std::find
#include <deque>
#include <functional>  // For std::hash
#include <initializer_list>
#include <iostream>
#include <memory>       // For std::allocator
#include <ranges>       // For ranges
#include <type_traits>  // For std::is_same
#include <unordered_set>
#include <cassert>      // For std::assert

namespace containerofunique {

template <class T, class Hash = std::hash<T>, class KeyEqual = std::equal_to<T>,
          class Allocator = std::allocator<T> >
// To do list 3: is it necessary to have _Pred and _Alloc?
// Do not use _Pred and _Alloc for now and only focus on dequeofuniqueints for
// now
class dequeofunique {
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

  using __deque_type = std::deque<T, Allocator>;
  using _deque_iterator = typename __deque_type::iterator;
  using _deque_const_iterator =
      typename __deque_type::const_iterator;
  using _deque_reverse_iterator =
      typename __deque_type::reverse_iterator;
  using _deque_const_reverse_iterator =
      typename __deque_type::const_reverse_iterator;
  using _deque_reference = typename __deque_type::reference;

  using __unordered_set_type = std::unordered_set<T, Hash, KeyEqual, Allocator>;
  using _unordered_set_iterator =
      typename __unordered_set_type::iterator;
  using _unordered_set_const_iterator =
      typename __unordered_set_type::const_iterator;
  using _set_reference =
      typename __unordered_set_type::reference;
  
  // To do list 9: modifty the iterator and make sure that when the element was
  // changed by deque_iterator, the set will get updated.
  
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

  const_iterator cbegin() const noexcept { return const_iterator(deque_.cbegin(), set_); }
  const_iterator cend() const noexcept { return const_iterator(deque_.cend(), set_); }
  
  const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(deque_.crbegin(), set_); }
  const_reverse_iterator crend() const noexcept { return const_reverse_iterator(deque_.crend(), set_); }

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

  // To do list 8: do I need to consider the Hash of other? I don't think it
  // makes sense to include the Hash of other since the dequeofunique is
  // supposed to store elements with same type and Hash.
  bool push_back(const dequeofunique<T, Hash>& other) {
    return push_back(other.deque_);
  }

  bool push_back(const std::deque<T>& other) {
    bool any_added = false;
    for (const auto& entry : other) {
      auto added = push_back(entry);
      any_added = any_added || added;
    }
    return any_added;
  }

  __deque_type deque() { return deque_; }

  __unordered_set_type set() { return set_; }

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
    std::cout << ".\n";
    std::cout << "Size of set is: " << set_.size() << ".\n";
    std::cout << "\n";
  }

private:
  __deque_type deque_;
  __unordered_set_type set_;
}; // class dequeofunique
}  // namespace containerofunique

int main() {
  std::cout << "Test constructors and operator '=' using int:\n";
  containerofunique::dequeofunique<int> dq_int_empty;
  std::cout << "Print dq_int_empty:\n";
  dq_int_empty.print();

  containerofunique::dequeofunique<int> dq_int_init1{1};
  std::cout << "Print dq_int_init1:\n";
  dq_int_init1.print();

  containerofunique::dequeofunique<int> dq_int_init2{1, 2};
  std::cout << "Print dq_int_init2:\n";
  dq_int_init2.print();

  containerofunique::dequeofunique<int> dq_int_init3{1, 2, 2, 2};
  std::cout << "Print dq_int_init3:\n";
  dq_int_init3.print();

  containerofunique::dequeofunique<int> dq_int_init4(dq_int_init3);
  std::cout << "Print dq_int_init4:\n";
  dq_int_init4.print();

  containerofunique::dequeofunique<int> dq_int_init5(std::move(dq_int_init3));
  std::cout << "Print dq_int_init5:\n";
  dq_int_init5.print();
  std::cout << "Print dq_int_init3:\n";
  dq_int_init3.print();

  containerofunique::dequeofunique<int> dq_int_init6 = dq_int_init5;
  std::cout << "Print dq_int_init6:\n";
  dq_int_init6.print();

  containerofunique::dequeofunique<int> dq_int_init7 = std::move(dq_int_init5);
  std::cout << "Print dq_int_init7:\n";
  dq_int_init7.print();
  std::cout << "Print dq_int_init5:\n";
  dq_int_init5.print();

  containerofunique::dequeofunique<int> dq_int_init8 = {4, 2, 6, 2, 1};
  std::cout << "Print dq_int_init8:\n";
  dq_int_init8.print();

  std::cout << "Test push_back():\n";
  std::cout << "Print dq_int_init1:\n";
  dq_int_init1.print();

  dq_int_init1.push_back(1);
  std::cout << "Print dq_int_init1:\n";
  dq_int_init1.print();

  dq_int_init1.push_back(2);
  std::cout << "Print dq_int_init1:\n";
  dq_int_init1.print();

  dq_int_init1.push_back(dq_int_init8);
  std::cout << "Print dq_int_init1:\n";
  dq_int_init1.print();

  std::deque<int> dq_int = {8, 9, 2};
  dq_int_init1.push_back(dq_int);
  std::cout << "Print dq_int_init1:\n";
  dq_int_init1.print();

}
