find_package(GTest REQUIRED)

enable_testing()

include(GoogleTest)

add_subdirectory(include_test)

add_subdirectory(src_test)