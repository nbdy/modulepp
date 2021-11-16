add_library(TestModule SHARED TestModule.cpp)
set_target_properties(TestModule PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(TestModule dl pthread)
