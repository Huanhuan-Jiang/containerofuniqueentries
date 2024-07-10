#pragma once

#include <deque>
#include <unordered_set>

namespace containerofunique {
template <typename T, typename Hash = std::hash<T>>
class DequeOfUniqueEntries {
 public:
 using value_type = T;

 DequeOfUniqueEntries() = default;
 
}

}  // namespace containerofunique