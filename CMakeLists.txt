cmake_minimum_required(VERSION 3.16)

project(UFMNext
    VERSION 1.0.0
    DESCRIPTION "2D Drawing MDI Studio in ISO C++17 with Qt6"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable compiler warnings
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# Qt Automoc / Autorcc / Autouic
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# -----------------------------------------------------------------------------
# 1. Core ISO C++ 17 Library (Zero Qt dependency)
# -----------------------------------------------------------------------------
set(CORE_HEADERS
    include/ufmnext/core/Point.h
    include/ufmnext/core/Rect.h
    include/ufmnext/core/Color.h
    include/ufmnext/core/ShapeType.h
    include/ufmnext/core/ConnectionPoint.h
    include/ufmnext/core/IDrawingContext.h
    include/ufmnext/core/Shape.h
    include/ufmnext/core/SquareShape.h
    include/ufmnext/core/RoundRectangleShape.h
    include/ufmnext/core/CircleShape.h
    include/ufmnext/core/OvalShape.h
    include/ufmnext/core/LineShape.h
    include/ufmnext/core/StarShape.h
    include/ufmnext/core/ArrowShape.h
    include/ufmnext/core/Tracker.h
    include/ufmnext/core/Document.h
    include/ufmnext/core/Serializer.h
)

set(CORE_SOURCES
    src/core/Shape.cpp
    src/core/SquareShape.cpp
    src/core/RoundRectangleShape.cpp
    src/core/CircleShape.cpp
    src/core/OvalShape.cpp
    src/core/LineShape.cpp
    src/core/StarShape.cpp
    src/core/ArrowShape.cpp
    src/core/Tracker.cpp
    src/core/Document.cpp
    src/core/Serializer.cpp
)

add_library(ufmnext_core STATIC ${CORE_HEADERS} ${CORE_SOURCES})

target_include_directories(ufmnext_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# -----------------------------------------------------------------------------
# 2. Qt6 GUI Application
# -----------------------------------------------------------------------------
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets PrintSupport)

set(GUI_HEADERS
    include/ufmnext/gui/QtDrawingContext.h
    include/ufmnext/gui/ShapeToolboxWidget.h
    include/ufmnext/gui/DrawingCanvas.h
    include/ufmnext/gui/MdiChild.h
    include/ufmnext/gui/ThemeManager.h
    include/ufmnext/gui/MainWindow.h
)

set(GUI_SOURCES
    src/gui/QtDrawingContext.cpp
    src/gui/ShapeToolboxWidget.cpp
    src/gui/DrawingCanvas.cpp
    src/gui/MdiChild.cpp
    src/gui/ThemeManager.cpp
    src/gui/MainWindow.cpp
    src/gui/main.cpp
)

add_executable(UFMNext WIN32 ${GUI_HEADERS} ${GUI_SOURCES})

target_include_directories(UFMNext PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(UFMNext PRIVATE
    ufmnext_core
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::PrintSupport
)

# Visual Studio IDE grouping
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
    ${CORE_HEADERS} ${CORE_SOURCES} ${GUI_HEADERS} ${GUI_SOURCES}
)

# -----------------------------------------------------------------------------
# 3. Unit Tests
# -----------------------------------------------------------------------------
enable_testing()

add_executable(ufmnext_tests tests/test_main.cpp)

target_include_directories(ufmnext_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(ufmnext_tests PRIVATE
    ufmnext_core
)

add_test(NAME CoreUnitTests COMMAND ufmnext_tests)
