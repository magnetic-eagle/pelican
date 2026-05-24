set(ENABLE_TESTING OFF)
set(SYSTEM_PUGIXML OFF)
set(SYSTEM_EASYQT OFF)

if (PkgConfig_FOUND)
	pkg_check_modules(PugiXML IMPORTED_TARGET pugixml)
	pkg_check_modules(EasyQt IMPORTED_TARGET easyqt)

	if(PugiXML_FOUND)
		set(SYSTEM_PUGIXML ON)
		add_library(pugixml::pugixml INTERFACE IMPORTED GLOBAL)
		target_link_libraries(pugixml::pugixml INTERFACE PkgConfig::PugiXML)
	endif()

	if(EasyQt_FOUND)
		set(SYSTEM_EASYQT ON)
		add_library(easyqt::easyqt INTERFACE IMPORTED GLOBAL)
		target_link_libraries(easyqt::easyqt INTERFACE PkgConfig::EasyQt)
	endif()
endif()

if (NOT TARGET pugixml::pugixml AND NOT SYSTEM_PUGIXML)
	message(STATUS "Using system provided PugiXML library")
elseif (NOT PugiXML_FOUND)
	message(STATUS "PugiXML library not found in the system, building from source")
	add_subdirectory(pugixml)
endif()

if (NOT TARGET easyqt::easyqt AND NOT SYSTEM_EASYQT)
	message(STATUS "EasyQt library not found in the system, building from source")
	add_subdirectory(easyqt)
endif()

unset(ENABLE_TESTING)

add_subdirectory(argparse)

