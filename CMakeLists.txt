cmake_minimum_required(VERSION 3.15)
project(protobuf_demo)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Protobuf ──────────────────────────────────────────────────────────────
find_package(Protobuf REQUIRED)
message(STATUS "Using protobuf ${Protobuf_VERSION}")

# Generate C++ from .proto
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS proto/company.proto)

# ── Library (shared between app and tests) ───────────────────────────────
add_library(company_lib
    src/company_repository.cpp
    ${PROTO_SRCS}
)
target_include_directories(company_lib PUBLIC
    include
    ${CMAKE_CURRENT_BINARY_DIR}   # generated .pb.h lives here
    ${Protobuf_INCLUDE_DIRS}
)
target_link_libraries(company_lib PUBLIC ${Protobuf_LIBRARIES})

# ── Main executable ───────────────────────────────────────────────────────
add_executable(protobuf_demo src/main.cpp)
target_link_libraries(protobuf_demo PRIVATE company_lib)

# ── Google Test ───────────────────────────────────────────────────────────
include(FetchContent)
set(FETCHCONTENT_TRY_FIND_PACKAGE_MODE NEVER)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
if(MSVC)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(protobuf_demo_tests tests/test_company.cpp)
target_link_libraries(protobuf_demo_tests PRIVATE company_lib gtest_main)

include(GoogleTest)
gtest_discover_tests(protobuf_demo_tests)
