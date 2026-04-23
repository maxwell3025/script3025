cmake_minimum_required(VERSION 3.31)
project(script3025)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -pedantic)

enable_testing()

# Here, we configure the spdlog level
add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)

# Here, we add subdirectories
add_subdirectory(parser)
add_subdirectory(script3025_core)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  # Ensure testing is enabled
  include(CTest)

  if(WIN32)
    set(OPEN_COMMAND "start")
  elseif(APPLE)
    set(OPEN_COMMAND "open")
  else()
    set(OPEN_COMMAND "xdg-open")
  endif()

  add_custom_target(coverage
    COMMAND lcov --zerocounters --directory .
    COMMAND ${CMAKE_CTEST_COMMAND} -C ${CMAKE_BUILD_TYPE} || true
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND lcov --capture --directory . --output-file coverage.info --rc branch_coverage=1 --ignore-errors mismatch
    COMMAND genhtml coverage.info --output-directory coverage_report --rc genhtml_branch_coverage=1
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generating HTML coverage report..."
    COMMAND ${OPEN_COMMAND} coverage_report/index.html
  )

  add_dependencies(coverage parser_test script3025_core_test)
endif()