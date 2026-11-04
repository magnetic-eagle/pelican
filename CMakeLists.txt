cmake_minimum_required(VERSION 3.10)
project(pelican
	VERSION 1.0.0
	DESCRIPTION "Graphical photo and video clip managing tool"
	HOMEPAGE_URL "https://github.com/magnetic-eagle/pelican"
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS_DEBUGASAN
    "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -fno-omit-frame-pointer -g"
)

set(CMAKE_EXE_LINKER_FLAGS_DEBUGASAN
    "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=address,undefined"
)

find_package(PkgConfig)
find_package(FreeImagePlus REQUIRED)
find_package(exiv2 REQUIRED)
find_package(fmt REQUIRED)
find_package(pystring REQUIRED)
find_package(SQLiteCpp REQUIRED)
find_package(easyqt)
find_package(pugixml)
include(GNUInstallDirs)

find_package(Qt6 COMPONENTS Concurrent Core Gui UiTools Widgets REQUIRED)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SYSTEM_EASYQT OFF)
set(SYSTEM_PUGIXML OFF)

if (PkgConfig_FOUND)
	if (NOT TARGET pugixml::pugixml)
		pkg_check_modules(PugiXML IMPORTED_TARGET pugixml)
		if(PugiXML_FOUND)
			set(SYSTEM_PUGIXML ON)
			add_library(pugixml::pugixml INTERFACE IMPORTED GLOBAL)
			target_link_libraries(pugixml::pugixml INTERFACE PkgConfig::PugiXML)
		endif()
	endif()
	if (NOT TARGET easyqt::easyqt)
		pkg_check_modules(EasyQt IMPORTED_TARGET easyqt)
		if(EasyQt_FOUND)
			set(SYSTEM_EASYQT ON)
			add_library(easyqt::easyqt INTERFACE IMPORTED GLOBAL)
			target_link_libraries(easyqt::easyqt INTERFACE PkgConfig::EasyQt)
		endif()
	endif()
endif()

add_subdirectory(3rdparty)
add_subdirectory(data)
add_subdirectory(src)

configure_file(config.hxx.in config.hxx @ONLY)

