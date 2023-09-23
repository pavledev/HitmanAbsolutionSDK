cmake_minimum_required (VERSION 3.8)

file(GLOB_RECURSE SRC_FILES
	CONFIGURE_DEPENDS
	src/*.cpp
	src/*.c
	src/*.hpp
	src/*.h
)

add_library(DirectInputProxy SHARED
	${SRC_FILES}
)

set_target_properties(DirectInputProxy PROPERTIES
	OUTPUT_NAME "dinput8"
	LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_SOURCE_DIR}\"/modules/DInput8.def"
)

install(TARGETS DirectInputProxy
	RUNTIME DESTINATION bin
)