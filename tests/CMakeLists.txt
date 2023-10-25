cmake_minimum_required(VERSION 3.27)

project(
    paraTests
    LANGUAGES CXX
)

if(PROJECT_IS_TOP_LEVEL)
    find_package(para REQUIRED)
    enable_testing()
endif()

include(FetchContent)
FetchContent_Declare(
    snitch
    GIT_REPOSITORY https://github.com/cschreib/snitch.git
    GIT_TAG        v1.2.2
    GIT_SHALLOW    TRUE
    SYSTEM
    FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(snitch)

add_executable(para_tests
    src/para_test.cpp
)

target_link_libraries(para_tests PRIVATE para::para snitch::snitch nanobench::nanobench)

add_test(NAME para_tests COMMAND para_tests)

# Enable test binary to find the DLL if building a shared lib
if(BUILD_SHARED_LIBS)
    set_tests_properties(para_tests PROPERTIES
        ENVIRONMENT_MODIFICATION "PATH=path_list_append:$<TARGET_FILE_DIR:para::para>/"
    )
endif()