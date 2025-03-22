cmake_minimum_required(VERSION 3.31)
project(parser_test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_library(
  parser
  src/parser.cpp
)

target_include_directories(parser
  PRIVATE src
  PUBLIC include
)

add_executable(
  parser_test
  test/parser_test.cpp
)


target_link_libraries(parser_test
  parser
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(parser_test)
