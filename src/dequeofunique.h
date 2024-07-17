#pragma once

#include <deque>
#include <initializer_list>
#include <unordered_set>

namespace containerofunique {
template <class _Value, class _Hash = std::hash<_Value>,
          class _Pred = std::equal_to<_Value>,
          class _Alloc = std::allocator<_Value> >
class dequeofunique {
  std::deque<T> deque_;
  std::unordered_set<T, Hash> set_;

 public:
  using value_type = T;
  using key_type = T;
  using hasher = Hash;
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
  using _deque_iterator =
      typename std::deque<T, Allocator>::iterator using _deque_const_iterator =
          typename std::deque<T, Allocator>::const_iterator;
  using _unordered_set_iterator =
      typename std::unordered_set<T, Hash, Pred, Allocator>::iterator;
  using _unordered_set_const_iterator =
      typename std::unordered_set<T, Hash, Pred, Allocator>::const_iterator;
  using iterator = = _deque_iterator;
  using _deque_reverse_iterator =
      typename std::deque<T, Allocator>::reverse_iterator;
  using _deque_const_reverse_iterator =
      typename std::deque<T, Allocator>::const_reverse_iterator;
}
}  // namespace containerofunique
