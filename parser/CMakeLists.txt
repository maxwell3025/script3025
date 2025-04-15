cmake_minimum_required(VERSION 3.31)
project(parser)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        6910c9d9165801d8827d628cb72eb7ea9dd538c5
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        48bcf39a661a13be22666ac64db8a7f886f2637e
)

FetchContent_MakeAvailable(spdlog)

enable_testing()

add_library(parser INTERFACE
  include/parser.hpp
)

target_include_directories(parser
  INTERFACE include
)

target_link_libraries(parser
  INTERFACE logger3025
)

add_executable(parser_test
  test/parser_test.cpp
)

target_link_libraries(parser_test
  parser
  gcov
  GTest::gtest_main
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(parser_test
    PRIVATE -fprofile-arcs
    PRIVATE -ftest-coverage
  )
  target_link_options(parser_test
    PRIVATE -lgcov
  )
endif()

include(GoogleTest)
gtest_discover_tests(parser_test)
