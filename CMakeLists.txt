cmake_minimum_required(VERSION 3.17)
project(modulepp VERSION 1.3)

option(TESTS "Enable tests" ON)
option(MODULES "Build modules" OFF)
option(README "Build readme example" ON)

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

    add_executable(ModuleVersionTests tests/ModuleVersionTests.cpp)
    target_link_libraries(ModuleVersionTests dl gtest_main)

    add_executable(ModuleInformationTests tests/ModuleInformationTests.cpp)
    target_link_libraries(ModuleInformationTests dl gtest_main)

    add_executable(ModuleDependencyTests tests/ModuleDependencyTests.cpp)
    target_link_libraries(ModuleDependencyTests dl gtest_main)

    include(GoogleTest)

    gtest_discover_tests(ModuleVersionTests)
    gtest_discover_tests(ModuleInformationTests)
    gtest_discover_tests(ModuleDependencyTests)
endif()

if(README)
    add_executable(ReadMe tests/ReadMe.cpp)
    target_link_libraries(ReadMe dl gtest_main)
    add_subdirectory(modules/TestModule)
    add_dependencies(ReadMe TestModule)
endif()

if(MODULES)
    add_subdirectory(modules)
endif()
