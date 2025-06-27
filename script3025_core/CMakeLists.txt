cmake_minimum_required(VERSION 3.31)
project(script3025_core)

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


add_library(script3025_core
  src/application_expression.cpp
  src/cst_transformers.cpp
  src/definition.cpp
  src/expression.cpp
  src/id_expression.cpp
  src/inductive_definition.cpp
  src/lambda_expression.cpp
  src/let_expression.cpp
  src/pi_expression.cpp
  src/parsing_utility.cpp
  src/program.cpp
  src/token.cpp
  include/application_expression.hpp
  include/cst_transformers.hpp
  include/definition.hpp
  include/expression.hpp
  include/expression_factory.hpp
  include/expression_visitor.hpp
  include/id_expression.hpp
  include/inductive_definition.hpp
  include/lambda_expression.hpp
  include/let_expression.hpp
  include/parsing_utility.hpp
  include/pi_expression.hpp
  include/program.hpp
  include/script3025_core.hpp
  include/token.hpp
)

target_include_directories(script3025_core
  PRIVATE src
  PUBLIC include
)

target_link_libraries(script3025_core
  PUBLIC parser
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