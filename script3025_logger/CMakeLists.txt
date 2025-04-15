cmake_minimum_required(VERSION 3.31)
project(logger3025)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        48bcf39a661a13be22666ac64db8a7f886f2637e
)

FetchContent_MakeAvailable(spdlog)

add_library(logger3025
  src/logger3025.cpp
)

target_include_directories(logger3025
  PUBLIC include
)

target_link_libraries(logger3025
  spdlog
)
