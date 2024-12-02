#pragma once

#include <deque>
#include <functional>  // For std::hash
#include <initializer_list>
#include <optional>  // For std::nullopt
#include <unordered_set>
#include <utility>  // For std::swap

namespace containerofunique {

template <class T, class Hash = std::hash<T>, class KeyEqual = std::equal_to<T>>
class deque_of_unique {
 public:
  // *Member types
  using value_type = T;
  using key_type = T;
  using hasher = Hash;
  using key_equal = KeyEqual;
  using reference = value_type&;
  using const_reference = const value_type&;
  using DequeType = std::deque<T>;
  using UnorderedSetType = std::unordered_set<T, Hash, KeyEqual>;
  using size_type = typename DequeType::size_type;
  using iterator = typename DequeType::iterator;
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

  deque_of_unique(const deque_of_unique& other) { _push_back(other); }

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

 private:
  DequeType deque_;
  UnorderedSetType set_;
};  // class deque_of_unique
};  // namespace containerofunique