# CMake build for tests.
#
# See also the Makefile, which is currently more fully featured.


# Set cmake builtin variables before calling project(), otherwise the
# cmake-provided defaults will get in first!
set(CMAKE_BUILD_TYPE "Release" CACHE STRING
    "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel."
)

set(CXX_STD "c++11" CACHE STRING "Version of C++ standard in use")

# This project is infrastructure.  Warnings from common warning levels should
# be errors on all compilers, unless explicitly silenced.
if(NOT WIN32)
    set(CMAKE_CXX_FLAGS "-Wall -Werror -std=${CXX_STD}" CACHE STRING "Flags used by the compiler during all build types.")
else()
    set(CMAKE_CXX_FLAGS "-W3 -WX")
endif()

project(tinyformat)
cmake_minimum_required(VERSION 2.8)
option(COMPILE_SPEED_TEST FALSE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(tinyformat_test tinyformat_test.cpp)

if (COMPILE_SPEED_TEST)
    add_executable(tinyformat_speed_test tinyformat_speed_test.cpp)
endif ()

enable_testing()
add_test(NAME test COMMAND tinyformat_test)

add_custom_target(testall COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS tinyformat_test)

