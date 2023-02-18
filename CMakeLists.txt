cmake_minimum_required(VERSION 3.24)
project(MazeGeneration)

set(CMAKE_CXX_STANDARD 17)

set(GLFW_H /opt/homebrew/Cellar/glfw/3.3.8/include/GLFW)
include_directories(${GLFW_H})

set(GLFW_LINK /opt/homebrew/Cellar/glfw/3.3.8/lib/libglfw.3.dylib)
link_libraries(${GLFW_LINK})

set(SOURCE_FILES
        main.cpp)

add_executable(MazeGeneration ${SOURCE_FILES})

if (APPLE)
    target_link_libraries(MazeGeneration "-framework OpenGL")
    target_link_libraries(MazeGeneration "-framework GLUT")
endif ()