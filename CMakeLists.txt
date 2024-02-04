cmake_minimum_required(VERSION 3.22)
project(CPP_COURSE)

set(CMAKE_CXX_STANDARD 17)

include_directories(lib)

file(GLOB SOURCES "src/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME})
