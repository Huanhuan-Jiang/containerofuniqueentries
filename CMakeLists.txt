cmake_minimum_required(VERSION 3.29)
project(dequeofuniquetest VERSION 1.0)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enable necessary compile options
add_compile_options(-std=c++23)

cmake_policy(SET CMP0135 NEW)

set(GOOGLETEST_VERSION "1.14.0" CACHE STRING "Version of Google Test")

add_subdirectory(src)
add_subdirectory(thirdparty/googletest)

add_executable(dequeofuniquetest tests/test_dequeofunique.cpp)

target_link_libraries(dequeofuniquetest PRIVATE 
    GTest::gtest_main 
    GTest::gmock_main
)

target_link_libraries(dequeofuniquetest PRIVATE containerofunique)

enable_testing()

include(GoogleTest)
gtest_discover_tests(dequeofuniquetest)
