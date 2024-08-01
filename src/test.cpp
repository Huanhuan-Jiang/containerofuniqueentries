 #include <iostream>
#include <unordered_set>
#include <limits>

int main() {
    std::unordered_set<int> uset;

    for (int i = 0; i < 10000; ++i) {
        uset.insert(i);
    }

    float current_load_factor = uset.load_factor();
    std::cout << "Current load factor: " << current_load_factor << std::endl;
    std::cout << "Number of buckets: " << uset.bucket_count() << std::endl;
    std::cout << "Max number of buckets: " << uset.max_bucket_count() << std::endl;
    std::cout << "Number of elements: " << uset.size() << std::endl;
    std::cout << "Max size (theoretical maximum number of elements): " << uset.max_size() << std::endl;

    typedef std::ptrdiff_t difference_type;
    difference_type max_difference = std::numeric_limits<difference_type>::max();

    std::cout << "The maximum value of difference_type (std::ptrdiff_t) is: " << max_difference << std::endl;

    return 0;
}
