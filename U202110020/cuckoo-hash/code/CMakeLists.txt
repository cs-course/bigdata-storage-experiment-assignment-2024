cmake_minimum_required(VERSION 3.8)

project(cuckoo-hash)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/libs)

# add_library(cuckoo-hash STATIC src/cuckoo_hash_table.cpp)

# add third partys
add_subdirectory(third_partys/googletest)
add_subdirectory(third_partys/spdlog)

include_directories(${PROJECT_SOURCE_DIR}/third_partys/googletest/googletest/include)
include_directories(${PROJECT_SOURCE_DIR}/third_partys/spdlog/include)

set(TEST_FILES test/test.cpp)

add_executable(cuckoo-hash-test ${TEST_FILES})

target_link_libraries(cuckoo-hash-test gtest gtest_main)
