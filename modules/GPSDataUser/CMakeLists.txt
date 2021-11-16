add_library(GPSDataUser SHARED GPSDataUser.cpp)
target_link_libraries(GPSDataUser dl pthread)
