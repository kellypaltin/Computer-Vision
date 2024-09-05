cmake_minimum_required(VERSION 3.10)
project(Task3)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(Task3 main.cpp)
target_link_libraries(Task3 ${OpenCV_LIBS})

