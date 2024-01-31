# CMake build for tests.
#
# See also the Makefile, which is currently more fully featured on unix.
cmake_minimum_required(VERSION 3.14)
project(tinyformat LANGUAGES CXX)

# Define the target library
add_library(tinyformat INTERFACE)
target_compile_features(tinyformat INTERFACE cxx_std_11)
target_compile_options(tinyformat INTERFACE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Werror>
)
target_include_directories(tinyformat INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# Setup for testing
include(CTest)
if(BUILD_TESTING)
    add_executable(tinyformat_test tinyformat_test.cpp)
    target_link_libraries(tinyformat_test PRIVATE tinyformat)
    add_test(NAME tinyformat_test COMMAND tinyformat_test)

    add_executable(tinyformat_speed_test tinyformat_speed_test.cpp)
    target_link_libraries(tinyformat_speed_test PRIVATE tinyformat)

    foreach(impl printf iostreams tinyformat boost)
        add_test(NAME tinyformat_speed_test_${impl} COMMAND tinyformat_speed_test ${impl})
    endforeach()
endif()

# Setup for installation
install(TARGETS tinyformat EXPORT tinyformatTargets)
install(FILES tinyformat.h DESTINATION include)
install(FILES tinyformatConfig.cmake DESTINATION lib/cmake/tinyformat)
install(
    EXPORT tinyformatTargets
    FILE tinyformatTargets.cmake
    DESTINATION lib/cmake/tinyformat
)
