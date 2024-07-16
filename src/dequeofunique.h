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
  using value_type = T;
  using key_type = T;
  using hasher =Hash;
  using key_equal = Pred;
  using allocator_type = Allocator;
 
  static_assert(is_same<typename allocator_type::value_type, value_type>::value,
              "Allocator::value_type must be same type as value_type");

  using __alloc_traits = allocator_traits<Allocator>;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = typename __alloc_traits::pointer;
  using size_type = typename __alloc_traits::size_type;
  using difference_type = typename __alloc_traits::difference_type;
  using _deque_iterator = typename std::deque<T, Allocator>::iterator
  using _deque_const_iterator = typename std::deque<T, Allocator>::const_iterator;
  using _unordered_set_iterator = typename std::unordered_set<T, Hash, Pred, Allocator>::iterator;
  using _unordered_set_const_iterator = typename std::unordered_set<T, Hash, Pred, Allocator>::const_iterator;
  using iterator = = _deque_iterator;
  using _deque_reverse_iterator = typename std::deque<T, Allocator>::reverse_iterator;
  using _deque_const_reverse_iterator = typename std::deque<T, Allocator>::const_reverse_iterator;

  _deque_iterator deque_begin() { return deque_.begin(); }
  _deque_iterator deque_end() { return deque_.end(); }
  _deque_const_iterator deque_cbegin() const { return deque_.begin(); }
  _deque_const_iterator deque_cend() const { return deque_.end(); }

  _deque_iterator deque_rbegin() { return deque_.rbegin(); }
  _deque_iterator deque_rend() { return deque_.rend(); }
  _deque_const_iterator deque_crbegin() const { return deque_.rbegin(); }
  _deque_const_iterator deque_crend() const { return deque_.rend(); }

  _unordered_set_iterator set_begin() { return set_.begin(); }
  _unordered_set_iterator set_end() { return set_.end(); }
  _unordered_set_const_iterator set_begin() const { return set_.begin(); }
  _unordered_set_const_iterator set_end() const { return set_.end(); }

  iterator begin() { return deque_begin(); }
  iterator end() { return deque_end(); }
  const_iterator cbegin() const { return deque_cbegin(); }
  const_iterator cend() const { return deque_cend(); }

  iterator rbegin() { return deque_rbegin(); }
  iterator rend() { return deque_rend(); }
  const_iterator crbegin() const { return deque_crbegin(); }
  const_iterator crend() const { return deque_crend(); }
}
}  // namespace containerofunique
