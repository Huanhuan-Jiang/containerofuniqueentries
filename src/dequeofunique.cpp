// #pragma once

#include <algorithm>  // For std::find
#include <cassert>    // For std::assert
#include <deque>
#include <functional>  // For std::hash
#include <initializer_list>
#include <iostream>
#include <memory>       // For std::allocator
#include <optional>     // For std::nullopt
#include <ranges>       // For ranges
#include <type_traits>  // For std::is_same
#include <unordered_set>

namespace containerofunique {

template <class T, class Hash = std::hash<T>, class KeyEqual = std::equal_to<T>,
          class Allocator = std::allocator<T>>
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
  using _deque_const_iterator = typename __deque_type::const_iterator;
  using _deque_reverse_iterator = typename __deque_type::reverse_iterator;
  using _deque_const_reverse_iterator =
      typename __deque_type::const_reverse_iterator;
  using _deque_reference = typename __deque_type::reference;

  using __unordered_set_type = std::unordered_set<T, Hash, KeyEqual, Allocator>;
  using _unordered_set_iterator = typename __unordered_set_type::iterator;
  using _unordered_set_const_iterator =
      typename __unordered_set_type::const_iterator;
  using _set_reference = typename __unordered_set_type::reference;

  using iterator = _deque_iterator;
  using const_iterator = _deque_const_iterator;
  using reverse_iterator = _deque_reverse_iterator;
  using const_reverse_iterator = _deque_const_reverse_iterator;

  // Member functions
  // Constructor
  dequeofunique() = default;

  // To do list 5:
  explicit dequeofunique(const Allocator& alloc);  // Can be done later
  // End of to do list 5:

  template <class InputIt>
  dequeofunique(InputIt first, InputIt last,
                const Allocator& alloc = Allocator()) {
    __push_back(first, last);
  }

  dequeofunique(const std::initializer_list<T>& init,
                const Allocator& alloc = Allocator())
      : dequeofunique(init.begin(), init.end()) {}

  dequeofunique(const dequeofunique& other) = default;

  dequeofunique(const dequeofunique& other, const Allocator& alloc);

  dequeofunique(dequeofunique&& other) {
    std::swap(deque_, other.deque_);
    std::swap(set_, other.set_);
  };

  dequeofunique(dequeofunique&& other, const Allocator& alloc);

  // To do list 2:
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

  // Element access
  // at: If pos is not within the range of the container, an exception of type
  // std::out_of_range is thrown.
  // []: Accessing a nonexistent element through this operator is undefined
  // behavior. Calling front on an empty container causes undefined behavior.
  // Calling back on an empty container causes undefined behavior.
  const_reference at(size_type pos) const { return deque_.at(pos); }
  const_reference front() const { return deque_.front(); }
  const_reference operator[](size_type pos) const { return deque_[pos]; }
  const_reference back() const { return deque_.back(); }

  // Iterators
  const_iterator cbegin() const noexcept { return deque_.cbegin(); }
  const_iterator cend() const noexcept { return deque_.cend(); }

  const_reverse_iterator crbegin() const noexcept { return deque_.crbegin(); }
  const_reverse_iterator crend() const noexcept { return deque_.crend(); }

  // Modifiers
  void clear() noexcept {
    deque_.clear();
    set_.clear();
  }

  const_iterator erase(const_iterator pos) {
    set_.erase(*pos);
    return deque_.erase(pos);
  }

  const_iterator erase(const_iterator first, const_iterator last) {
    for (const_iterator it = first; it != last; ++it) {
      set_.erase(*it);
    }
    return deque_.erase(first, last);
  }

  std::pair<const_iterator, bool> insert(const_iterator pos, const T& value) {
    if (set_.insert(value).second) {
      return std::make_pair(deque_.insert(pos, value), true);
    }
    return std::make_pair(pos, false);
  }

  std::pair<const_iterator, bool> insert(const_iterator pos, T&& value) {
    if (set_.insert(value).second) {
      return std::make_pair(deque_.insert(pos, std::move(value)), true);
    }
    return std::make_pair(pos, false);
  };

  template <class InputIt>
  std::pair<const_iterator, bool> insert(const_iterator pos, InputIt first,
                                         InputIt last) {
    bool any_added = false;
    int num_inserted = 0;
    auto return_it = pos;
    for (const_iterator it = first; it != last; ++it) {
      auto added = set_.insert(*it).second;
      if (added) {
        auto tmp_it = deque_.insert(pos, *it);
        if (++num_inserted == 1) {
          return_it = tmp_it;
        }
      }
      any_added = any_added || added;
    }
    return (std::make_pair(return_it, any_added));
  }

  std::pair<const_iterator, bool> insert(const_iterator pos,
                                         std::initializer_list<T> ilist) {
    bool any_added = false;
    int num_inserted = 0;
    auto return_it = pos;
    for (auto it = ilist.begin(); it != ilist.end(); ++it) {
      auto added = set_.insert(*it).second;
      if (added) {
        auto tmp_it = deque_.insert(pos, *it);
        if (++num_inserted == 1) {
          return_it = tmp_it;
        }
      }
      any_added = any_added || added;
    }
    return (std::make_pair(return_it, any_added));
  }

  template <class... Args>
  std::pair<const_iterator, bool> emplace(const_iterator pos, Args&&... args) {
    if (set_.emplace(std::forward<Args>(args)...).second) {
      return (std::make_pair(deque_.emplace(pos, std::forward<Args>(args)...),
                             true));
    }
    return (std::make_pair(pos, false));
  }

  template <class... Args>
  std::optional<std::reference_wrapper<T>> emplace_front(Args&&... args) {
    if (set_.emplace(std::forward<Args>(args)...).second) {
      return deque_.emplace_front(std::forward<Args>(args)...);
    }
    return std::nullopt;
  }

  template <class... Args>
  std::optional<std::reference_wrapper<T>> emplace_back(Args&&... args) {
    if (set_.emplace(std::forward<Args>(args)...).second) {
      return deque_.emplace_back(std::forward<Args>(args)...);
    }
    return std::nullopt;
  }

  void pop_front() {
    auto f = deque_.front();
    deque_.pop_front();
    set_.erase(f);
  }

  void pop_back() {
    auto f = deque_.back();
    deque_.pop_back();
    set_.erase(f);
  }

  bool push_front(const T& value) {
    if (set_.insert(value).second) {
      deque_.push_front(value);
      return true;
    }
    return false;
  }

  bool push_front(T&& value) {
    if (set_.insert(value).second) {
      deque_.push_front(value);
      return true;
    }
    return false;
  }

  bool push_back(const T& value) {
    if (set_.insert(value).second) {
      deque_.push_back(value);
      return true;
    }
    return false;
  }

  bool push_back(T&& value) {
    if (set_.insert(value).second) {
      deque_.push_back(value);
      return true;
    }
    return false;
  }

  template <class InputIt>
  void __push_back(InputIt first, InputIt last) {
    while (first != last) {
      push_back(*first++);
    }
  }

  bool __push_back(const dequeofunique<T, Hash>& other) {
    return __push_back(other.deque_);
  }

  bool __push_back(const std::deque<T>& other) {
    bool any_added = false;
    for (const auto& entry : other) {
      auto added = push_back(entry);
      any_added = any_added || added;
    }
    return any_added;
  }

  // template< container-compatible-range<T> R >
  // void append_range( R&& rg );

  // void resize( size_type count );
  // void resize( size_type count, const value_type& value );

  // template< container-compatible-range<T> R >
  // void prepend_range( R&& rg );

  void swap(dequeofunique& other) noexcept {
    deque_.swap(other.deque_);
    set_.swap(other.set_);
  };

  // Capacity
  bool empty() const noexcept {
    if (deque_.size() == 0) {
      return true;
    }
    return false;
  };

  size_type size() const noexcept { return (deque_.size()); }

  // To do list 10:
  // size_type max_size() const noexcept {};
  // void shrink_to_fit();
  // End of to do list 10

  // Destructor
  ~dequeofunique() = default;

  // Get member variables
  __deque_type deque() { return deque_; }
  __unordered_set_type set() { return set_; }

  void print() const {
    std::cout << "Print out the dequeofunique.\n";
    std::cout << "Size of dequeofunique is: " << size() << ".\n";
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
};  // class dequeofunique
}  // namespace containerofunique

int main() {
  std::cout << "Test constructors and operator '=' using int:\n";
  containerofunique::dequeofunique<int> dq_int_empty;
  if (dq_int_empty.size() == 0) {
    std::cout << "dq_int_empty is empty.\n";
    std::cout << "Print dq_int_empty:\n";
    dq_int_empty.print();
  }

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
  std::cout << "Print dq_int_init3 after move:\n";
  dq_int_init3.print();

  containerofunique::dequeofunique<int> dq_int_init6 = dq_int_init5;
  std::cout << "Print dq_int_init6:\n";
  dq_int_init6.print();

  containerofunique::dequeofunique<int> dq_int_init7 = std::move(dq_int_init5);
  std::cout << "Print dq_int_init7:\n";
  dq_int_init7.print();
  std::cout << "Print dq_int_init5 after move:\n";
  dq_int_init5.print();

  std::deque<int> deque_int = {1, 2, 3, 4, 5};
  containerofunique::dequeofunique<int> dq_int_init8(deque_int.begin(),
                                                     deque_int.end());
  std::cout << "Print dq_int_init8:\n";
  dq_int_init8.print();

  containerofunique::dequeofunique<int> dq = {1, 2, 3, 4};
  std::cout << "Print dq:\n";
  dq.print();

  std::cout << "Test element access:\n";
  std::cout << "The first element of dq_int1: " << dq.front() << ".\n";
  std::cout << "The second element of dq_int1: " << dq.at(1) << ".\n";
  std::cout << "The third element of dq_int1: " << dq[2] << ".\n";
  std::cout << "The last element of dq_int1: " << dq.back() << ".\n";

  std::cout << "\n";
  std::cout << "Test iterators using int:\n";
  std::cout << "The first element of dq is: " << *dq.cbegin() << ".\n";
  auto it = dq.cbegin();
  std::cout << "The second element of dq is: " << *(++it) << ".\n";
  std::cout << "The second element of dq is: " << *it++ << ".\n";
  std::cout << "The third element of dq is: " << *it << ".\n";
  std::cout << "The second element of dq is: " << *(--it) << ".\n";
  std::cout << "The second element of dq is: " << *it-- << ".\n";
  std::cout << "The third element of dq is: " << *(--dq.cend()) << ".\n";

  std::cout << "\nTest reverse iterators using int: \n";
  std::cout << "The first element of reversed dq is : " << *dq.crbegin()
            << ".\n";
  auto rit = dq.crbegin();
  std::cout << "The second element of reversed dq is : " << *(++rit) << ".\n";
  std::cout << "The second element of reversed dq is : " << *rit++ << ".\n";
  std::cout << "The third element of reversed dq is : " << *rit << ".\n";
  std::cout << "The second element of reversed dq is : " << *(--rit) << ".\n";
  std::cout << "The second element of reversed dq is : " << *rit-- << ".\n";
  std::cout << "The third element of reversed dq is : " << *--dq.crend()
            << ".\n";

  std::cout << "\nTest modifiers using int: \n";
  dq.clear();
  std::cout << "Print dq after clear: \n";
  dq.print();
  std::cout << "\nTest push_front and push_back using int:\n";
  dq.push_back(3);
  int int1 = 4;
  dq.push_back(std::move(int1));
  dq.push_front(2);
  int int2 = 1;
  dq.push_front(std::move(int2));
  dq.print();

  dq.clear();
  dq = containerofunique::dequeofunique<int>({1, 2, 3, 4});
  dq.pop_front();
  std::cout << "Print dq after pop_front: \n";
  dq.print();
  dq.pop_back();
  std::cout << "Print dq after pop_back: \n";
  dq.print();

  dq.clear();
  dq = containerofunique::dequeofunique<int>({1, 2, 3, 4});
  std::cout << "Print dq after erase one element:" << *dq.erase(dq.cbegin())
            << "\n";
  dq.print();
  dq.erase(dq.cbegin(), dq.cbegin() + 2);
  std::cout << "Print dq after erase a range of elements: \n";
  dq.print();

  int value = 1;
  if (dq.insert(dq.cbegin(), value).second) {
    std::cout << "Print dq after insert one lvalue 1: \n";
    dq.print();
  }

  if (dq.insert(dq.cbegin(), value).second) {
    std::cout << "Print dq after insert one lvalue 1 again: \n";  // no printing
    dq.print();
  }

  int value2 = 2;
  if (dq.insert(dq.cbegin() + 1, std::move(value2)).second) {
    std::cout << "Print dq after insert one rvalue 2: \n";
    dq.print();
  }

  auto values = containerofunique::dequeofunique<int>({1, 2, 3, 4});
  dq.clear();
  if (dq.insert(dq.cbegin(), values.cbegin(), values.cbegin() + 2).second) {
    std::cout << "Print dq after insert muliple elements: \n";
    dq.print();
  }
  values.clear();
  values = containerofunique::dequeofunique<int>({2, 5, 7, 8});
  if (dq.insert(dq.cbegin(), values.cbegin(), values.cbegin() + 3).second) {
    std::cout
        << "Print dq after insert muliple elements again: \n";  // no printing
    dq.print();
  }
  if (dq.insert(dq.cbegin(), {1, 2, 3, 4}).second) {
    std::cout << "Print dq after insert an initilizer list: \n";
    dq.print();
  }

  dq.clear();
  dq = containerofunique::dequeofunique<int>({1, 2, 3, 4});
  dq.emplace(dq.cbegin() + 2, 2);
  std::cout << "Emplace with 2:\n";
  dq.print();
  dq.emplace(dq.cbegin() + 3, 7);
  std::cout << "Emplace with 7:\n";
  dq.print();

  dq.clear();
  dq = containerofunique::dequeofunique<int>({1, 2, 3, 4});
  dq.emplace_back(2);
  std::cout << "Emplace_back with 2:\n";
  dq.print();
  dq.emplace_back(7);
  std::cout << "Emplace_back with 7:\n";
  dq.print();

  dq.clear();
  dq = containerofunique::dequeofunique<int>({1, 2, 3, 4});
  dq.emplace_front(2);
  std::cout << "Emplace_front with 2:\n";
  dq.print();
  dq.emplace_front(7);
  std::cout << "Emplace_front with 7:\n";
  dq.print();

  containerofunique::dequeofunique<int> dq1({1, 2, 3, 4});
  containerofunique::dequeofunique<int> dq2({5, 6, 7, 8});
  std::cout << "Before swap:\n";
  dq1.print();
  dq2.print();
  dq1.swap(dq2);
  std::cout << "After swap:\n";
  dq1.print();
  dq2.print();

  return 0;
}
