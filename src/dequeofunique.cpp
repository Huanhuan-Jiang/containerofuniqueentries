// #pragma once

#include <algorithm>  // For std::find
#include <cassert>    // For std::assert
#include <compare>    // For std::weak_ordering
#include <deque>
#include <functional>  // For std::hash
#include <initializer_list>
#include <iostream>
#include <memory>       // For std::allocator
#include <optional>     // For std::nullopt
#include <ranges>       // For ranges
#include <type_traits>  // For std::is_same
#include <unordered_set>
#include <utility>

namespace containerofunique {

template <class T, class Hash = std::hash<T>, class KeyEqual = std::equal_to<T>,
          class Allocator = std::allocator<T>>

class deque_of_unique {
 public:
  // *Member types
  using value_type = T;
  using key_type = T;
  using hasher = Hash;
  using key_equal = KeyEqual;
  using allocator_type = Allocator;

  static_assert(std::is_same_v<typename allocator_type::value_type, value_type>,
                "Allocator::value_type must be same type as value_type");

  using AllocTraits = std::allocator_traits<Allocator>;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = typename AllocTraits::pointer;
  using size_type = typename AllocTraits::size_type;
  using difference_type = typename AllocTraits::difference_type;

  using deque_type = std::deque<T, Allocator>;
  using UnorderedSetType = std::unordered_set<T, Hash, KeyEqual, Allocator>;

  using iterator = typename deque_type::iterator;
  using const_iterator = typename deque_type::const_iterator;
  using reverse_iterator = typename deque_type::reverse_iterator;
  using const_reverse_iterator = typename deque_type::const_reverse_iterator;
  using _deque_reference = typename deque_type::reference;

  // Member functions
  // Constructor
  deque_of_unique() = default;

  template <class input_it>
  deque_of_unique(input_it first, input_it last) {
    _push_back(first, last);
  }

  deque_of_unique(const std::initializer_list<T>& init)
      : deque_of_unique(init.begin(), init.end()) {}

  deque_of_unique(const deque_of_unique& other) = default;

  deque_of_unique(deque_of_unique&& other) noexcept {
    std::swap(deque_, other.deque_);
    std::swap(set_, other.set_);
  };

  deque_of_unique& operator=(const deque_of_unique& other) = default;
  deque_of_unique& operator=(deque_of_unique&& other) noexcept {
    std::swap(deque_, other.deque_);
    std::swap(set_, other.set_);
    return *this;
  };
  deque_of_unique& operator=(std::initializer_list<T> ilist) {
    deque_of_unique temp(ilist);
    std::swap(deque_, ilist.deque_);
    std::swap(set_, ilist.set_);
    return *this;
  };

  // Element access
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
    for (auto it = first; it != last; ++it) {
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

  template <class input_it>
  std::pair<const_iterator, bool> insert(const_iterator pos, input_it first,
                                         input_it last) {
    bool any_added = false;
    int num_inserted = 0;
    auto return_it = pos;
    for (auto it = first; it != last; ++it) {
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
    return (insert(pos, ilist.begin(), ilist.end()));
  }

  template <class... Args>
  std::pair<const_iterator, bool> emplace(const_iterator pos, Args&&... args) {
    if (set_.emplace(args...).second) {
      return (std::make_pair(deque_.emplace(pos, std::forward<Args>(args)...),
                             true));
    }
    return (std::make_pair(pos, false));
  }

  template <class... Args>
  std::optional<std::reference_wrapper<T>> emplace_front(Args&&... args) {
    if (set_.emplace(args...).second) {
      return deque_.emplace_front(std::forward<Args>(args)...);
    }
    return std::nullopt;
  }

  template <class... Args>
  std::optional<std::reference_wrapper<T>> emplace_back(Args&&... args) {
    if (set_.emplace(args...).second) {
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
    auto temp = std::move(value);
    if (set_.insert(temp).second) {
      deque_.push_front(std::move(temp));
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
    auto temp = std::move(value);
    if (set_.insert(temp).second) {
      deque_.push_back(std::move(temp));
      return true;
    }
    return false;
  }

  template <class input_it>
  void _push_back(input_it first, input_it last) {
    while (first != last) {
      push_back(*first++);
    }
  }

  bool _push_back(const deque_of_unique<T, Hash>& other) {
    return _push_back(other.deque_);
  }

  bool _push_back(const std::deque<T>& other) {
    bool any_added = false;
    for (const auto& entry : other) {
      auto added = push_back(entry);
      any_added = any_added || added;
    }
    return any_added;
  }

  void swap(deque_of_unique& other) noexcept {
    deque_.swap(other.deque_);
    set_.swap(other.set_);
  };

  // Capacity
  bool empty() const noexcept { return deque_.empty(); };

  size_type size() const noexcept { return (deque_.size()); }

  // operators
  auto operator<=>(const deque_of_unique& other) const {
    return (deque_ <=> other.deque_);
  }

  bool operator==(const deque_of_unique& other) const {
    return (deque_ == other.deque_);
  }

  // Destructor
  ~deque_of_unique() = default;

  // Get member variables
  deque_type deque() { return deque_; }
  UnorderedSetType set() { return set_; }

  void print() const {
    std::cout << "Print out the deque_of_unique.\n";
    std::cout << "Size of deque_of_unique is: " << size() << ".\n";
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
  deque_type deque_;
  UnorderedSetType set_;
};  // class deque_of_unique
}  // namespace containerofunique

int main() noexcept {
  std::cout << "Test constructors and operator '=' using int:\n";
  containerofunique::deque_of_unique<int> dq_int_empty;
  if (dq_int_empty.empty() == false) {
    std::cout << "dq_int_empty is empty.\n";
    std::cout << "Print dq_int_empty:\n";
    dq_int_empty.print();
  }

  containerofunique::deque_of_unique<int> dq_int_init1{1};
  std::cout << "Print dq_int_init1:\n";
  dq_int_init1.print();

  containerofunique::deque_of_unique<int> dq_int_init2{1, 2};
  std::cout << "Print dq_int_init2:\n";
  dq_int_init2.print();

  containerofunique::deque_of_unique<int> dq_int_init3{1, 2, 2, 2};
  std::cout << "Print dq_int_init3:\n";
  dq_int_init3.print();

  containerofunique::deque_of_unique<int> dq_int_init4(dq_int_init3);
  std::cout << "Print dq_int_init4:\n";
  dq_int_init4.print();

  containerofunique::deque_of_unique<int> dq_int_init5(std::move(dq_int_init3));
  std::cout << "Print dq_int_init5:\n";
  dq_int_init5.print();

  containerofunique::deque_of_unique<int> dq_int_init6 = dq_int_init5;
  std::cout << "Print dq_int_init6:\n";
  dq_int_init6.print();

  containerofunique::deque_of_unique<int> dq_int_init7 =
      std::move(dq_int_init5);
  std::cout << "Print dq_int_init7:\n";
  dq_int_init7.print();

  std::deque<int> deque_int = {1, 2, 3, 4, 5};
  containerofunique::deque_of_unique<int> dq_int_init8(deque_int.begin(),
                                                       deque_int.end());
  std::cout << "Print dq_int_init8:\n";
  dq_int_init8.print();

  containerofunique::deque_of_unique<int> dq = {1, 2, 3, 4};
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
  dq = containerofunique::deque_of_unique<int>({1, 2, 3, 4});
  dq.pop_front();
  std::cout << "Print dq after pop_front: \n";
  dq.print();
  dq.pop_back();
  std::cout << "Print dq after pop_back: \n";
  dq.print();

  dq.clear();
  dq = containerofunique::deque_of_unique<int>({1, 2, 3, 4});
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

  auto values = containerofunique::deque_of_unique<int>({1, 2, 3, 4});
  dq.clear();
  if (dq.insert(dq.cbegin(), values.cbegin(), values.cbegin() + 2).second) {
    std::cout << "Print dq after insert muliple elements: \n";
    dq.print();
  }
  values.clear();
  values = containerofunique::deque_of_unique<int>({2, 5, 7, 8});
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
  dq = containerofunique::deque_of_unique<int>({1, 2, 3, 4});
  dq.emplace(dq.cbegin() + 2, 2);
  std::cout << "Emplace with 2:\n";
  dq.print();
  dq.emplace(dq.cbegin() + 3, 7);
  std::cout << "Emplace with 7:\n";
  dq.print();

  dq.clear();
  dq = containerofunique::deque_of_unique<int>({1, 2, 3, 4});
  dq.emplace_back(2);
  std::cout << "Emplace_back with 2:\n";
  dq.print();
  dq.emplace_back(7);
  std::cout << "Emplace_back with 7:\n";
  dq.print();

  dq.clear();
  dq = containerofunique::deque_of_unique<int>({1, 2, 3, 4});
  dq.emplace_front(2);
  std::cout << "Emplace_front with 2:\n";
  dq.print();
  dq.emplace_front(7);
  std::cout << "Emplace_front with 7:\n";
  dq.print();

  containerofunique::deque_of_unique<int> dq1({1, 2, 3, 4});
  containerofunique::deque_of_unique<int> dq2({5, 6, 7, 8});
  std::cout << "Before swap:\n";
  dq1.print();
  dq2.print();
  dq1.swap(dq2);
  std::cout << "After swap:\n";
  dq1.print();
  dq2.print();

  dq1.clear();
  dq2.clear();
  dq1 = containerofunique::deque_of_unique<int>({1, 2, 3, 4});
  dq2 = containerofunique::deque_of_unique<int>({5, 6, 7, 8});
  containerofunique::deque_of_unique<int> dq3(dq1);
  if (dq1 == dq3) {
    std::cout << "dq1 is equal to dq3\n";
  }
  if (dq1 == dq2) {
    std::cout << "dq1 is not equal to dq2\n";
  }
  if (dq1 < dq2) {
    std::cout << "dq1 is less than dq2\n";
  }
  if (dq1 <= dq2) {
    std::cout << "dq1 is less than or equal to dq2\n";
  }
  if (dq2 > dq1) {
    std::cout << "dq2 is greater than dq1\n";
  }
  if (dq2 >= dq1) {
    std::cout << "dq2 is greater than or equal to dq1\n";
  }

  if ((dq1 <=> dq2) == std::weak_ordering::less) {
    std::cout << "dq1 is less than dq2.\n";
  }
  if ((dq2 <=> dq1) == std::weak_ordering::greater) {
    std::cout << "dq2 is greater than dq1.\n";
  }
  if ((dq1 <=> dq3) == std::weak_ordering::equivalent) {
    std::cout << "dq1 is equivalent to dq3.\n";
  }

  return 0;
}
