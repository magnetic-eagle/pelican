add_subdirectory(ui)

install(
	FILES pelican.desktop
	DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
)
install(
	FILES settings.xml
	DESTINATION ${CMAKE_INSTALL_DATADIR}/pelican
)

set(ICON_NAME "pelican")
set(ICON_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/${ICON_NAME}.svg")

install(
	FILES "${ICON_SOURCE}"
	DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps"
)

find_program(INKSCAPE_EXECUTABLE inkscape)

if (INKSCAPE_EXECUTABLE)
	foreach(SIZE IN ITEMS 48 256)
		set(OUTPUT
			"${CMAKE_CURRENT_BINARY_DIR}/${ICON_NAME}-${SIZE}.png"
		)

		add_custom_command(
			OUTPUT "${OUTPUT}"
			COMMAND "${INKSCAPE_EXECUTABLE}"
			"${ICON_SOURCE}"
			--export-filename=${OUTPUT}
			--export-width=${SIZE}
			--export-height=${SIZE}
			DEPENDS "${ICON_SOURCE}"
			VERBATIM
		)

		list(APPEND GENERATED_ICONS "${OUTPUT}")
	endforeach()

	add_custom_target(pelican-icons ALL
		DEPENDS ${GENERATED_ICONS}
	)

	install(FILES
		"${CMAKE_CURRENT_BINARY_DIR}/${ICON_NAME}-48.png"
		DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/48x48/apps"
		RENAME "${ICON_NAME}.png"
	)

	install(FILES
		"${CMAKE_CURRENT_BINARY_DIR}/${ICON_NAME}-256.png"
		DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/256x256/apps"
		RENAME "${ICON_NAME}.png"
	)
else()
	message(WARNING "The inkscape executable was not found - cannot generate PNG theme icons; this may cause missing icons for some desktop environments")
endif()
