PROJECT(order-book)
cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER "g++-10")
set(CMAKE_CXX_EXTENSIONS OFF)

# Options to build demo and test
option(BUILD_DEMO "Build the demo" OFF)
option(BUILD_TEST "Build the test" OFF)

# ============================
# Directories
# ============================

set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(SRC_DIR "${ROOT_DIR}/src")
set(INCLUDE_DIR "${ROOT_DIR}/include")
set(DEMO_DIR "${ROOT_DIR}/demo")
set(TEST_DIR "${ROOT_DIR}/test")
set(THIRD_PARTY_DIR "${ROOT_DIR}/3rd")
set(CONFIG_DIR "${ROOT_DIR}/config")

# ============================
# Includes
# ============================

include_directories(
    ${INCLUDE_DIR}
    ${SRC_DIR}
)

# ============================
# Libraries
# ============================

# link_directories(
#     ${THIRD_PARTY_DIR}/protobuf/src/.libs
#     ${THIRD_PARTY_DIR}/llbc/output/gmake/release64
# )

# ============================
# Pakcages
# ============================



# ============================
# Files
# ============================

# file(GLOB_RECURSE CONFIG_FILES ${CONFIG_DIR}/*)
# file(COPY ${CONFIG_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/config)

# ============================
# Build
# ============================

add_subdirectory(include)
add_subdirectory(src)

# Conditionally build demo
if (BUILD_DEMO)
    add_subdirectory(demo)
    add_custom_target(demo ALL
        DEPENDS lbook
    )
endif()

# Conditionally build test
if (BUILD_TEST)
    add_subdirectory(test)
    add_custom_target(test ALL
        DEPENDS lutil lbook
    )
endif()