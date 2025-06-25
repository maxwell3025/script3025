cmake_minimum_required(VERSION 3.31)
project(script3025)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

# Here, we configure the spdlog level
add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)

# Here, we add subdirectories
add_subdirectory(parser)
add_subdirectory(script3025_core)
