cmake_minimum_required(VERSION 3.17)
project(modulepp)

option(TESTS "Enable tests" OFF)
option(MODULES "Build modules" ON)

set(CMAKE_CXX_STANDARD 17)

add_library(modulepp SHARED include/modulepp.h)
set_target_properties(modulepp PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(modulepp dl pthread)

add_library(moduleppstatic SHARED include/modulepp.h)
set_target_properties(moduleppstatic PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(moduleppstatic dl pthread)

include_directories(include)

if(TESTS)
    include(FetchContent)
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    enable_testing()

    add_executable(modulepp_tests tests.cpp)
    target_compile_definitions(modulepp_tests PRIVATE -DTESTS)
    target_link_libraries(modulepp_tests dl gtest_main)
    include(GoogleTest)
    gtest_discover_tests(modulepp_tests)
endif()

if(MODULES)
    add_subdirectory(modules)
endif()
