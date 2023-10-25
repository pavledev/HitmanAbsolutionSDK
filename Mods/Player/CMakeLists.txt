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

add_library(Player SHARED
	${HEADER_FILES}
	${SRC_FILES}
)

target_include_directories(Player PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(Player PRIVATE
	HitmanAbsolutionSDK
)

install(TARGETS Player
	RUNTIME DESTINATION bin/mods
)