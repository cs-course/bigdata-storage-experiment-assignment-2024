cmake_minimum_required(VERSION 3.20)
project(cuckoo-hash)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

add_subdirectory(deps)
add_library(
        cuckoo
        src/cuckoo.cpp
        src/data_generater.cpp
        src/block_cuckoo.cpp
        src/stash_cuckoo.cpp
        src/stash_blocked_cuckoo.cpp
)
add_executable(
        ${PROJECT_NAME}
        main.cpp
        include/init.h
)
target_link_libraries(
        ${PROJECT_NAME}
        cuckoo
        external
)