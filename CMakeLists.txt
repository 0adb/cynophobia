
# 3.14 chosen for FetchContent_MakeAvailable
# (earliest version)
cmake_minimum_required(VERSION 3.14...3.30)

project(
  Cynophobia
  VERSION 0.1
  DESCRIPTION "A compiler for a subset of C"
  LANGUAGES CXX)

include(FetchContent)
 
add_subdirectory(src)
 
add_subdirectory(apps)
 
add_subdirectory(tests)
