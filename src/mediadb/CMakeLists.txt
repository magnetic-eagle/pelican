add_library(pelican-mediadb STATIC
	collection.cxx
	database.cxx
	exifutils.cxx
	importer.cxx
	media.cxx
)

target_include_directories(
	pelican-mediadb
	PUBLIC
		.
		${FREEIMAGEPLUS_INCLUDE_DIRS}
)

target_link_libraries(
	pelican-mediadb
	easyqt::easyqt
	exiv2lib
	fmt
	${FREEIMAGEPLUS_LIBRARIES}
)
