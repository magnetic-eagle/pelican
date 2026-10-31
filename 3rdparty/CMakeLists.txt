set(ENABLE_TESTING OFF)

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

