# CMakeLists.txt for building tests
enable_testing()
find_package(GTest)

# Set up the test target
add_executable(tests
    test_main.cpp  # Your test source files
    test_classes.cpp 
)

# Link against your library and Google Test
target_link_libraries(tests PRIVATE reservation_sys gtest gtest_main )

# Register tests with CTest
add_test(NAME ReservationUnitTests COMMAND tests)