cmake_minimum_required(VERSION 3.27)

project(
    para
    VERSION 0.1.0
    DESCRIPTION "A short description"
    LANGUAGES CXX
)

include(cmake/variables.cmake)

# Declare library

add_library(
    para_para
    src/single_queue.cpp
    src/multi_queue.cpp
)
add_library(para::para ALIAS para_para)

include(cmake/dependencies.cmake)
include(cmake/compile-options.cmake)

include(GenerateExportHeader)
generate_export_header(
    para_para
    BASE_NAME para
    EXPORT_FILE_NAME export/para/export.hpp
    CUSTOM_CONTENT_FROM_VARIABLE pragma_suppress_c4251
)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(para_para PUBLIC para_STATIC_DEFINE)
endif()

set_target_properties(
    para_para PROPERTIES
    CXX_VISIBILITY_PRESET "hidden"
    VISIBILITY_INLINES_HIDDEN YES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    EXPORT_NAME para
    OUTPUT_NAME para
)

target_include_directories(
    para_para ${warning_guard}
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_include_directories(
    para_para SYSTEM
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/export>"
)

# Benchmarks
if(PARA_BENCHMARK)
    add_subdirectory(benchmark)
endif(PARA_BENCHMARK)

# Tests
if(PARA_BUILD_TESTS)
    include(CTest)
    add_subdirectory(tests)
endif()

# Install rules

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()
