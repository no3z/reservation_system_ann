cmake_minimum_required(VERSION 3.20)

project(ReservationSystem)

# Add your C++ standard version here
set(CMAKE_CXX_STANDARD 11)

# Add the source files for your library and executable
add_subdirectory(src/lib)  # Add this line

# Create an executable from main.cpp and link with your library
find_package(asio 1.28.1 REQUIRED CONFIG)
find_package(jsoncpp REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/src/lib)
add_executable(ReservationSystem ${CMAKE_SOURCE_DIR}/src/app/main.cpp)
target_link_libraries(ReservationSystem PRIVATE reservation_sys asio::asio JsonCpp::JsonCpp)  # Link your library here

# Optionally, install the executable
install(TARGETS ReservationSystem DESTINATION bin)

option(RUN_TESTS "Build the tests" ON)
if(RUN_TESTS)
	add_subdirectory(src/tests) 
endif()

option(RUN_DOXYGEN "Build doxygen documentation" ON)
if(RUN_DOXYGEN)
	add_subdirectory(docs)  
endif()