cmake_minimum_required(VERSION 3.31)
project(script3025_core)

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

add_library(script3025_core
  src/script3025_core.cpp
  src/parsing_utility.cpp
)

target_include_directories(script3025_core
  PRIVATE src
  PUBLIC include
)

target_link_libraries(script3025_core
  parser
)

add_executable(script3025_core_test
  test/script3025_core_test.cpp
)

target_link_libraries(script3025_core_test
  script3025_core
  gcov
  GTest::gtest_main
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(script3025_core_test
    PRIVATE -fprofile-arcs
    PRIVATE -ftest-coverage
  )
  target_link_options(script3025_core_test
    PRIVATE -lgcov
  )
endif()

enable_testing()

include(GoogleTest)
gtest_discover_tests(script3025_core_test)