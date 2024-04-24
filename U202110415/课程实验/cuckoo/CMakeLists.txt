cmake_minimum_required(VERSION 3.0)
project(CuckooMap)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)
find_package(BZip2 REQUIRED)

#list(APPEND CMAKE_CXX_FLAGS "-fprofile-arcs -ftest-coverage -DNDEBUG ${CMAKE_CXX_FLAGS}")
#list(APPEND CMAKE_EXE_LINKER_FLAGS "-O0 -g -fprofile-arcs -ftest-coverage -DNDEBUG ${CMAKE_EXE_LINKER_FLAGS}")


add_library(cuckoo INTERFACE
        include/CuckooMap/CuckooMap.h
        include/CuckooMap/InternalCuckooMap.h
        include/CuckooMap/qdigest.h)
target_include_directories(cuckoo INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_executable(CuckooFilterTest
    tests/CuckooFilterTest.cpp
)

add_executable(PerformanceTest
        tests/PerformanceTest.cpp

)
target_link_libraries(CuckooFilterTest PRIVATE cuckoo)

target_link_libraries(PerformanceTest PRIVATE cuckoo)



