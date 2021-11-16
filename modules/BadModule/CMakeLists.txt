add_library(BadModule SHARED BadModule.cpp)
set_target_properties(BadModule PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(BadModule dl pthread)
