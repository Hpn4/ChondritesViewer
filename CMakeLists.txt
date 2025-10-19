cmake_minimum_required(VERSION 3.16)
project(SEMViewer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGLWidgets)
find_package(PkgConfig REQUIRED)
pkg_check_modules(VIPS REQUIRED vips-cpp)

include_directories(${VIPS_INCLUDE_DIRS})
link_directories(${VIPS_LIBRARY_DIRS})

add_executable(sem_viewer
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/ImageView.cpp
    src/ImageView.h
    src/ImageTileLoader.cpp
    src/ImageTileLoader.h
    src/SyncController.cpp
    src/SyncController.h
)

target_link_libraries(sem_viewer
    Qt6::Widgets
    Qt6::OpenGLWidgets
    ${VIPS_LIBRARIES}
)

