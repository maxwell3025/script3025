cmake_minimum_required(VERSION 3.31)
project(script3025)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Although testing is enabled in the individual subdirectories, 
enable_testing()

# Here, we forward the spdlog active level to compilation
add_compile_definitions(SPDLOG_ACTIVE_LEVEL=${SPDLOG_ACTIVE_LEVEL})
add_compile_options(-fdiagnostics-color)

# Here, we add subdirectories
add_subdirectory(parser)
add_subdirectory(script3025_core)
