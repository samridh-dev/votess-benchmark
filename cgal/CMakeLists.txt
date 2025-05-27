cmake_minimum_required(VERSION 3.10)

project(benchmark_cgal LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "compiler id: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "c++ standard: c++${CMAKE_CXX_STANDARD}")

find_package(CGAL REQUIRED)
find_package(TBB  REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -O3")

if (CMAKE_CXX_COMPILER MATCHES "icpx$")
      add_compile_options(-fsycl -qopenmp)
      add_link_options(-fsycl -qopenmp)
else()
      add_compile_options(-fopenmp)
      add_link_options(-fopenmp)
endif()

add_compile_definitions(CGAL_LINKED_WITH_TBB)

include_directories(../include)

file(GLOB CXX_SRC "*.cpp")
if (NOT CXX_SRC)
  message(FATAL_ERROR "no source files found.")
endif()

add_executable(benchmark_cgal ${CXX_SRC})

target_link_libraries(benchmark_cgal PRIVATE
        CGAL::CGAL
        TBB::tbb
        TBB::tbbmalloc
)
