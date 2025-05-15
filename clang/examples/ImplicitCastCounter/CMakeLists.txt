cmake_minimum_required(VERSION 3.22)
project(ImplicitCastCounter)

set(CLANG_DIR /usr/lib/llvm-17/lib/cmake/clang)
set(LLVM_LIBRARY_DIRS "/usr/lib/llvm-17/lib")
link_directories(${LLVM_LIBRARY_DIRS})

find_package(Clang REQUIRED CONFIG PATHS ${CLANG_DIR})
find_package(LLVM REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_library(ImplicitCastCounter MODULE ImplicitCastCounter.cpp)

set_target_properties(ImplicitCastCounter PROPERTIES
  CXX_STANDARD 17
  PREFIX ""
  SUFFIX ".so"
)

target_link_libraries(ImplicitCastCounter
  PRIVATE
    clangAST
    clangASTMatchers
    clangBasic
    clangFrontend
)
