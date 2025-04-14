cmake_minimum_required(VERSION 3.31)
project(script3025_logger)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        48bcf39a661a13be22666ac64db8a7f886f2637e
)

FetchContent_MakeAvailable(spdlog)

add_library(script3025_logger INTERFACE)

target_include_directories(script3025_logger
  INTERFACE include
)

target_link_libraries(script3025_logger
  INTERFACE spdlog
)
