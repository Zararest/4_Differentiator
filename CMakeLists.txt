cmake_minimum_required(VERSION 2.8)

add_executable(Differ src/headers/Differ.h src/headers/DSL.h src/headers/Node.h src/Differ.cpp src/Node.cpp src/main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)