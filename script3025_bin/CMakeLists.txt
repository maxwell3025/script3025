cmake_minimum_required(VERSION 3.31)
project(script3025_bin)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(script3025
  src/main.cpp
)

target_include_directories(script3025
  PRIVATE src
  PUBLIC include
)

target_link_libraries(script3025
  PUBLIC script3025_core
)
