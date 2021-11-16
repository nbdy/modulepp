add_library(GPS SHARED GPS.cpp)
target_link_libraries(GPS dl pthread gps)
