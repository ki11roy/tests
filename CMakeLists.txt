# CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
cmake_minimum_required (VERSION 2.8.11)
project (TEST)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++0x)
ELSEIF(CMAKE_COMPILER_IS_GNUCC)
    #disable gcc warning
ENDIF()

add_executable(test test.cpp)
add_executable(test2 test2.cpp)
add_executable(test3 test3.cpp)

                           