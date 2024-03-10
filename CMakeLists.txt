cmake_minimum_required(VERSION 3.22)
project(CPP_COURSE)

set(CMAKE_CXX_STANDARD 17)

# Encuentra el paquete wxWidgets
find_package(wxWidgets REQUIRED core base stc aui)

# Incluye los directorios de wxWidgets
include(${wxWidgets_USE_FILE})

include_directories(lib)

file(GLOB SOURCES "src/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCES})

# Enlaza las bibliotecas de wxWidgets con tu ejecutable
target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})