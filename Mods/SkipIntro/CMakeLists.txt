cmake_minimum_required(VERSION 3.8)

file(GLOB_RECURSE SRC_FILES
	CONFIGURE_DEPENDS
	src/*.cpp
	src/*.c
)

add_library(SkipIntro SHARED
	${HEADER_FILES}
	${SRC_FILES}
)

target_include_directories(SkipIntro PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(SkipIntro PRIVATE
	HitmanAbsolutionSDK
)

install(TARGETS SkipIntro
	RUNTIME DESTINATION bin/mods
)