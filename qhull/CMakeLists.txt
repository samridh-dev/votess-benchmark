cmake_minimum_required(VERSION 3.10)

project(benchmark_qhull LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "compiler id: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "c++ standard: c++${CMAKE_CXX_STANDARD}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -O3")

if (CMAKE_CXX_COMPILER MATCHES "icpx$")
      add_compile_options(-fsycl -qopenmp)
      add_link_options(-fsycl -qopenmp)
endif()

add_subdirectory(extern/qhull)
include_directories(../include)

file(GLOB CXX_SRC "*.cpp")
if (NOT CXX_SRC)
  message(FATAL_ERROR "no source files found.")
endif()

add_executable(benchmark_qhull ${CXX_SRC})

target_link_libraries(benchmark_qhull PRIVATE
        qhullstatic_r qhullcpp
)

target_include_directories(benchmark_qhull PRIVATE
    ${CMAKE_SOURCE_DIR}/extern/qhull/src
)
