cmake_minimum_required(VERSION 3.8)

file(GLOB_RECURSE HEADER_FILES
	CONFIGURE_DEPENDS
	include/*.h
)

file(GLOB_RECURSE SRC_FILES
	CONFIGURE_DEPENDS
	src/*.cpp
	src/*.c
)

add_library(Editor SHARED
	${HEADER_FILES}
	${SRC_FILES}
)

target_include_directories(Editor PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(Editor PRIVATE
	HitmanAbsolutionSDK
	imguizmo
	imgui-node-editor
)

install(TARGETS Editor
	RUNTIME DESTINATION bin/mods
)