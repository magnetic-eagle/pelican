add_subdirectory(ui)

install(
    FILES pelican.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
)
