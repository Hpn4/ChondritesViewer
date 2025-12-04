include(FetchContent)

cmake_minimum_required(VERSION 3.16)
project(SEMViewer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

cmake_policy(SET CMP0135 NEW)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)

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
    src/ImageLoader.cpp
    src/ImageLoader.h
    src/SharedGLResources.h
    src/SharedGLResources.cpp
    src/ImageTransform.h
    src/modules/ModuleBase.cpp
    src/modules/ModuleBase.h
    src/modules/PaintVImage.cpp
    src/modules/PaintVImage.h
    src/modules/PaintLabel.cpp
    src/modules/PaintLabel.h
    src/modules/ImageLabel.cpp
    src/modules/ImageLabel.h
    src/widgets/ColorLabelList.h
    src/widgets/ColorLabelList.cpp
    src/widgets/CollapsibleWidget.h
    src/widgets/CollapsibleWidget.cpp
)

target_link_libraries(sem_viewer
    Qt6::Widgets
    Qt6::OpenGLWidgets
    nlohmann_json::nlohmann_json
    ${VIPS_LIBRARIES}
)

