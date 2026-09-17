cmake_minimum_required(VERSION 3.20)

project(minic_compiler LANGUAGES C CXX)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
find_package(LLVM REQUIRED CONFIG)

if(NOT LLVM_VERSION_MAJOR EQUAL 18)
  message(FATAL_ERROR "LLVM 18 is required; found LLVM ${LLVM_PACKAGE_VERSION}")
endif()

bison_target(
  MiniCParser
  src/parser.y
  ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp
  DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.hpp
)

flex_target(
  MiniCLexer
  src/lexer.l
  ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp
)

add_flex_bison_dependency(MiniCLexer MiniCParser)

add_executable(
  compiler
  src/compiler.cpp
  src/ast.cpp
  src/analysis.cpp
  src/ir_builder.cpp
  src/optimizer.cpp
  src/backend.cpp
  ${BISON_MiniCParser_OUTPUTS}
  ${FLEX_MiniCLexer_OUTPUTS}
)

target_compile_features(compiler PRIVATE cxx_std_17)
target_compile_options(compiler PRIVATE -Wall -Wextra -Wpedantic)

target_include_directories(
  compiler
  PRIVATE
    include
    ${CMAKE_CURRENT_BINARY_DIR}
    ${LLVM_INCLUDE_DIRS}
)

llvm_map_components_to_libnames(LLVM_LIBRARIES analysis core irreader support)
target_link_libraries(compiler PRIVATE ${LLVM_LIBRARIES})

set_target_properties(compiler PROPERTIES OUTPUT_NAME compiler.out)

set_source_files_properties(
  ${FLEX_MiniCLexer_OUTPUTS}
  PROPERTIES COMPILE_OPTIONS -Wno-unused-function
)

add_executable(
  optimizer_runner
  tests/optimizer/runner.cpp
  src/optimizer.cpp
)

target_compile_features(optimizer_runner PRIVATE cxx_std_17)
target_compile_options(optimizer_runner PRIVATE -Wall -Wextra -Wpedantic)
target_include_directories(optimizer_runner PRIVATE include ${LLVM_INCLUDE_DIRS})
target_link_libraries(optimizer_runner PRIVATE ${LLVM_LIBRARIES})
