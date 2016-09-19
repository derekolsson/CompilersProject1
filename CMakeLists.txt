cmake_minimum_required(VERSION 3.5)
project(project1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES src/main.cpp lib/utilities.cpp lib/utilities.h lib/Lexer.cpp lib/Lexer.h lib/AutomataState.cpp lib/AutomataState.h src/testSuite.cpp src/testSuite.h)
add_executable(project1 ${SOURCE_FILES})
