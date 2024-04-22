add_subdirectory(
        argparse
)
add_library(
        external INTERFACE
)
include_directories(
        argparse/include
)
target_link_libraries(
        external INTERFACE
        argparse
)