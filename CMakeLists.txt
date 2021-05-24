cmake_minimum_required(VERSION 2.8)

project(OpenGLWidget)

find_package(OpenGL REQUIRED)

find_package(PkgConfig REQUIRED)

pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(example
    main.cpp
    OpenGLWidget.cpp
    OpenGLWidget.h
    trianglerenderer.cpp
    trianglerenderer.h)

target_include_directories(example PRIVATE   ${GTKMM_INCLUDE_DIRS})
link_directories(  ${GTKMM_LIBRARY_DIRS}  )
target_link_libraries(example PRIVATE ${GTKMM_LIBRARIES} ${OPENGL_LIBRARIES})
target_link_libraries(example PRIVATE "epoxy")
