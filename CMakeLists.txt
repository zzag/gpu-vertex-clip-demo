cmake_minimum_required(VERSION 3.0)
project(gpu-vertex-clip-demo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 CONFIG REQUIRED COMPONENTS
    Core
    Gui
    OpenGL
    OpenGLWidgets
    Widgets
)

add_executable(gpu-vertex-clip-demo
    main.cpp
    window.cpp
)
target_link_libraries(gpu-vertex-clip-demo
    Qt::Core
    Qt::Gui
    Qt::OpenGL
    Qt::OpenGLWidgets
    Qt::Widgets
)
