cmake_minimum_required(VERSION 3.13.0)

project(OpenGL VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(my_drawing)
target_link_libraries(my_drawing PRIVATE "-framework Cocoa" "-framework OpenGL" "-framework IOKit" "-framework CoreVideo" "-framework QuartzCore" "-framework AppKit" /Users/rusher/Lib/glfw-3.5.1/libglfw3.a)
target_include_directories(my_drawing PRIVATE ~/Include ./include)
if(TYPE STREQUAL app)
    target_sources(my_drawing PRIVATE main.cc)
else()
    add_subdirectory(src)
endif()
target_sources(my_drawing PRIVATE glad.c)