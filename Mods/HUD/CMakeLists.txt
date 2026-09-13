cmake_minimum_required(VERSION 3.8)

file(GLOB_RECURSE SRC_FILES
	CONFIGURE_DEPENDS
	Src/*.cpp
  Src/*.c
  Src/*.hpp
  Src/*.h
)

add_library(HUD SHARED
	${SRC_FILES}
)

target_include_directories(HUD PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/Src
)

target_link_libraries(HUD PRIVATE
	HitmanAbsolutionSDK
)
