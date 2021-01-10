cmake_minimum_required(VERSION 3.17)
project(modulepp)

set(CMAKE_CXX_STANDARD 11)

add_library(modulepp SHARED modulepp.h)
set_target_properties(modulepp PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(modulepp dl pthread)

add_library(moduleppstatic SHARED modulepp.h)
set_target_properties(moduleppstatic PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(moduleppstatic dl pthread)

add_library(test_module SHARED modulepp.h tests/TestModule.cpp)
set_target_properties(test_module PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(test_module dl pthread)

add_executable(test_module_test modulepp.h tests/test.cpp)
target_link_libraries(test_module_test dl pthread)