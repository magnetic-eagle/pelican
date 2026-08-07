cmake_minimum_required(VERSION 3.5)
project(pelican
	VERSION 1.0.0
	DESCRIPTION "Graphical photo and video clip managing tool"
	HOMEPAGE_URL "https://github.com/TheFGFSEagle/pelican"
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig)
find_package(FreeImagePlus REQUIRED)
find_package(exiv2 REQUIRED)
find_package(fmt REQUIRED)
find_package(pystring REQUIRED)

find_package(Qt6 COMPONENTS Concurrent Core Gui UiTools Widgets REQUIRED)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(3rdparty)
add_subdirectory(src)

configure_file(config.hxx.in config.hxx @ONLY)

