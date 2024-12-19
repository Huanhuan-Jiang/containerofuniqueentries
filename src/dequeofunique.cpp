// #pragma once

// clang-format off
#include <deque>
#include <functional>  // For std::hash
#include <initializer_list>
#include <optional>  // For std::nullopt
#include <unordered_set>
#include <utility>  // For std::swap

#include <algorithm>  // For for_each
#include <compare>    // For std::weak_ordering
#include <iostream>
// clang-format on

namespace containerofunique {

template <class T, class Hash = std::hash<T>, class KeyEqual = std::equal_to<T>>
class deque_of_unique {
 public:
  // *Member types
  using value_type = T;
  using key_type = T;
  using hasher = Hash;
  using key_equal = KeyEqual;
  using const_reference = const value_type&;
  using DequeType = std::deque<T>;
  using UnorderedSetType = std::unordered_set<T, Hash, KeyEqual>;
  using size_type = typename DequeType::size_type;
  using const_iterator = typename DequeType::const_iterator;
  using reverse_iterator = typename DequeType::reverse_iterator;
  using const_reverse_iterator = typename DequeType::const_reverse_iterator;

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
  }

  deque_of_unique& operator=(const deque_of_unique& other) = default;
  deque_of_unique& operator=(deque_of_unique&& other) noexcept = default;
  deque_of_unique& operator=(std::initializer_list<T> ilist) {
    deque_of_unique temp(ilist);
    std::swap(deque_, temp.deque_);
    std::swap(set_, temp.set_);
    return *this;
  }

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
  }

  template <class input_it>
  const_iterator insert(const_iterator pos, input_it first, input_it last) {
    auto return_it = pos;
    for (auto it = first; it != last; ++it) {
      if (set_.insert(*it).second) {
        return_it = deque_.insert(pos, *it);
      }
    }
    return return_it;
  }

  const_iterator insert(const_iterator pos, std::initializer_list<T> ilist) {
    return insert(pos, ilist.begin(), ilist.end());
  }

  template <class... Args>
  std::pair<const_iterator, bool> emplace(const_iterator pos, Args&&... args) {
    if (set_.emplace(args...).second) {
      return std::make_pair(deque_.emplace(pos, std::forward<Args>(args)...),
                            true);
    }
    return std::make_pair(pos, false);
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
    const auto& f = deque_.front();
    deque_.pop_front();
    set_.erase(f);
  }

  void pop_back() {
    const auto& f = deque_.back();
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
  }

  // Capacity
  bool empty() const noexcept { return deque_.empty(); }

  size_type size() const noexcept { return deque_.size(); }

  // operators
  auto operator<=>(const deque_of_unique& other) const {
    return deque_ <=> other.deque_;
  }

  // Destructor
  ~deque_of_unique() = default;

  // Get member variables
  const DequeType& deque() const { return deque_; }
  const UnorderedSetType& set() const { return set_; }

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
  DequeType deque_;
  UnorderedSetType set_;
};  // class deque_of_unique
};  // namespace containerofunique

int main() noexcept {
  std::cout << "Test constructors and operator '=' using int:\n";
  std::cout << "Test empty().\n";
  containerofunique::deque_of_unique<int> dq_int_empty;
  if (dq_int_empty.empty() == false) {
    std::cout << "dq_int_empty is empty.\n";
    std::cout << "Print dq_int_empty:\n";
    dq_int_empty.print();
  }

  std::cout << "Test deque_of_unique(const std::initializer_list<T>& init).\n";
  containerofunique::deque_of_unique<int> dq_int_init1{1};
  std::cout << "Print dq_int_init1:\n";
  dq_int_init1.print();

  containerofunique::deque_of_unique<int> dq_int_init2{1, 2};
  std::cout << "Print dq_int_init2:\n";
  dq_int_init2.print();

  containerofunique::deque_of_unique<int> dq_int_init3{1, 2, 2, 2};
  std::cout << "Print dq_int_init3:\n";
  dq_int_init3.print();

  std::cout << "Test deque_of_unique(const deque_of_unique& other).\n";
  containerofunique::deque_of_unique<int> dq_int_init4(dq_int_init3);
  std::cout << "Print dq_int_init4:\n";
  dq_int_init4.print();

  std::cout << "Test deque_of_unique(const deque_of_unique&& other).\n";
  containerofunique::deque_of_unique<int> dq_int_init5(std::move(dq_int_init3));
  std::cout << "Print dq_int_init5:\n";
  dq_int_init5.print();

  std::cout
      << "Test deque_of_unique& operator=(const deque_of_unique& other).\n";
  containerofunique::deque_of_unique<int> dq_int_init6 = dq_int_init5;
  std::cout << "Print dq_int_init6:\n";
  dq_int_init6.print();

  std::cout
      << "Test deque_of_unique& operator=(const deque_of_unique&& other).\n";
  containerofunique::deque_of_unique<int> dq_int_init7 =
      std::move(dq_int_init5);
  std::cout << "Print dq_int_init7:\n";
  dq_int_init7.print();

  std::cout << "Test deque_of_unique(input_it first, input_it last).\n";
  std::deque<int> deque_int = {1, 2, 3, 4, 5};
  containerofunique::deque_of_unique<int> dq_int_init8(deque_int.begin(),
                                                       deque_int.end());
  std::cout << "Print dq_int_init8:\n";
  dq_int_init8.print();

  std::cout
      << "Test deque_of_unique& operator=(std::initializer_list<T> ilist).\n";
  containerofunique::deque_of_unique<int> dq = {1, 2, 3, 4};
  std::cout << "Print dq:\n";
  dq.print();

  std::cout << "Test element access:\n";
  std::cout << "Test at(), front(), operator(), and back():\n";
  std::cout << "The first element of dq_int1: " << dq.front() << ".\n";
  std::cout << "The second element of dq_int1: " << dq.at(1) << ".\n";
  std::cout << "The third element of dq_int1: " << dq[2] << ".\n";
  std::cout << "The last element of dq_int1: " << dq.back() << ".\n";

  std::cout << "\n";
  std::cout << "Test iterators using int:\n";
  std::cout << "Test cbegin() and cend().\n";
  std::cout << "The first element of dq is: " << *dq.cbegin() << ".\n";
  auto it = dq.cbegin();
  std::cout << "The second element of dq is: " << *(++it) << ".\n";
  std::cout << "The second element of dq is: " << *it++ << ".\n";
  std::cout << "The third element of dq is: " << *it << ".\n";
  std::cout << "The second element of dq is: " << *(--it) << ".\n";
  std::cout << "The second element of dq is: " << *it-- << ".\n";
  std::cout << "The third element of dq is: " << *(--dq.cend()) << ".\n";

  std::cout << "\nTest reverse iterators using int: \n";
  std::cout << "Test rbegin() and crend().\n";
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
  std::cout << "\nTest clear().\n";
  dq.clear();
  std::cout << "Print dq after clear: \n";
  dq.print();
  std::cout << "\nTest push_front and push_back using int:\n";

  std::cout << "\nTest push_back(const T& value) and push_back(T&& value).\n";
  dq.push_back(3);
  int int1 = 4;
  dq.push_back(std::move(int1));

  std::cout << "\nTest push_front(const T& value) and push_front(T&& value).\n";
  dq.push_front(2);
  int int2 = 1;
  dq.push_front(std::move(int2));  // {1, 2, 3, 4}
  dq.print();

  std::cout << "\nTest pop_front() and pop_back().\n";
  dq.clear();
  dq = containerofunique::deque_of_unique<int>({1, 2, 3, 4});
  dq.pop_front();
  std::cout << "Print dq after pop_front: \n";  // {2, 3, 4}
  dq.print();
  dq.pop_back();
  std::cout << "Print dq after pop_back: \n";  // {2, 3}
  dq.print();

  std::cout << "\nTest clear(), erase(const_iterator pos), "
               "erase(const_iterator first, const_iterator last) \n";
  dq.clear();
  dq = containerofunique::deque_of_unique<int>({1, 2, 3, 4});
  std::cout << "Print dq after erase one element:" << *dq.erase(dq.cbegin())
            << "\n";
  dq.print();  // {2, 3, 4}
  dq.erase(dq.cbegin(), dq.cbegin() + 2);
  std::cout << "Print dq after erase a range of elements: \n";
  dq.print();  // {4}

  std::cout << "insert(const_iterator pos, const T& value), "
               "insert(const_iterator pos, T&& value), insert(const_iterator "
               "pos, input_it first, input_it last), and insert(const_iterator "
               "pos, std::initializer_list<T> ilist).\n";
  int value = 1;
  if (dq.insert(dq.cbegin(), value).second) {
    std::cout << "Print dq after insert one lvalue 1: \n";
    dq.print();  // {1, 4}
  }

  if (dq.insert(dq.cbegin(), value).second) {
    std::cout << "Print dq after insert one lvalue 1 again: \n";  // no printing
    dq.print();
  }

  int value2 = 2;
  if (dq.insert(dq.cbegin() + 1, std::move(value2)).second) {
    std::cout << "Print dq after insert one rvalue 2: \n";
    dq.print();  // {1, 2, 4}
  }

  auto values = containerofunique::deque_of_unique<int>({1, 2, 3, 4});
  dq.clear();
  if (dq.insert(dq.cbegin(), values.cbegin(), values.cbegin() + 2) !=
      dq.cbegin()) {
    std::cout << "Print dq after insert muliple elements: \n";
    dq.print();  // {1, 2}
  }
  values.clear();
  values = containerofunique::deque_of_unique<int>({2, 5, 7, 8});
  if (dq.insert(dq.cbegin(), values.cbegin(), values.cbegin() + 3) !=
      dq.cbegin()) {
    std::cout
        << "Print dq after insert muliple elements again: \n";  // no printing
    dq.print();
  }
  if (dq.insert(dq.cbegin(), {1, 2, 3, 4}) != dq.cbegin()) {
    std::cout << "Print dq after insert an initilizer list: \n";
    dq.print();
  }

  std::cout << "Test emplace(const_iterator pos, Args&&... args).\n";
  dq.clear();
  dq = containerofunique::deque_of_unique<int>({1, 2, 3, 4});
  dq.emplace(dq.cbegin() + 2, 2);
  std::cout << "Emplace with 2:\n";
  dq.print();
  int value3 = 7;
  dq.emplace(dq.cbegin() + 3, std::move(value3));
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

  std::cout << "\nTest emplace_back(Args&&... args).\n";
  dq.clear();
  dq = containerofunique::deque_of_unique<int>({1, 2, 3, 4});
  dq.emplace_front(2);
  std::cout << "Emplace_front with 2:\n";
  dq.print();
  int value4 = 7;
  dq.emplace_front(std::move(value4));
  std::cout << "Emplace_front with 7:\n";
  dq.print();

  std::cout << "\nTest swap(deque_of_unique& other).\n";
  containerofunique::deque_of_unique<int> dq1({1, 2, 3, 4});
  containerofunique::deque_of_unique<int> dq2({5, 6, 7, 8});
  std::cout << "Before swap:\n";
  dq1.print();
  dq2.print();
  dq1.swap(dq2);
  std::cout << "After swap:\n";
  dq1.print();
  dq2.print();

  std::cout << "\nTest size().\n";
  std::cout << "The size of dq2 is: " << dq2.size() << "\n";  // should be 4

  std::cout << "\nTest operator <=>.\n";
  dq1.clear();
  dq2.clear();
  dq1 = containerofunique::deque_of_unique<int>({1, 2, 3, 4});
  dq2 = containerofunique::deque_of_unique<int>({5, 6, 7, 8});
  containerofunique::deque_of_unique<int> dq3(dq1);
  if ((dq1 <=> dq3) == std::strong_ordering::equal) {
    std::cout << "dq1 is equal to dq3.\n";
  } else {
    std::cout << "dq1 is not equal to dq3.\n";
  }

  if ((dq1 <=> dq2) == std::strong_ordering::equal) {
    std::cout << "dq1 is  equal to dq2.\n";
  } else {
    std::cout << "dq1 is not equal to dq2.\n";
  }

  if (dq1 < dq2) {
    std::cout << "dq1 is less than dq2.\n";
  }
  if (dq1 <= dq2) {
    std::cout << "dq1 is less than or equal to dq2.\n";
  }
  if (dq2 > dq1) {
    std::cout << "dq2 is greater than dq1.\n";
  }
  if (dq2 >= dq1) {
    std::cout << "dq2 is greater than or equal to dq1.\n";
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

  std::cout << "\nTest getting member variables:\n";
  dq1.clear();
  dq1 = containerofunique::deque_of_unique<int>({1, 2, 3, 4});
  dq1.print();
  std::cout << "Get deque of dq1 using deque(): ";
  std::for_each(dq1.deque().begin(), dq1.deque().end(),
                [](const auto& num) { std::cout << num << " "; });
  std::cout << "\n";
  std::cout << "Get set of dq1 using set(): ";
  std::for_each(dq1.set().begin(), dq1.set().end(),
                [](const auto& num) { std::cout << num << " "; });
  std::cout << "\n";
  return 0;
}
