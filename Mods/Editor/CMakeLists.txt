cmake_minimum_required(VERSION 3.8)

file(GLOB_RECURSE SRC_FILES
	CONFIGURE_DEPENDS
	Src/*.cpp
  Src/*.c
  Src/*.hpp
  Src/*.h
)

add_library(Editor SHARED
	${SRC_FILES}
)

target_include_directories(Editor PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/Src
)

target_link_libraries(Editor PRIVATE
	HitmanAbsolutionSDK
	imguizmo
	imgui-node-editor
)

target_compile_definitions(Editor PRIVATE IMGUI_DEFINE_MATH_OPERATORS)
