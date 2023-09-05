add_library(reservation_sys
    classes.cpp
    classes.h
    reservation_system.h
    reservation_system.cpp
)
find_package(jsoncpp REQUIRED)

target_include_directories(reservation_sys PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(reservation_sys PRIVATE  JsonCpp::JsonCpp) 