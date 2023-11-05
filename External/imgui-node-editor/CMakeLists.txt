cmake_minimum_required(VERSION 3.8)

add_library(imgui-node-editor STATIC
	source/crude_json.cpp
	source/crude_json.h
	source/imgui_bezier_math.h
	source/imgui_bezier_math.inl
	source/imgui_canvas.cpp
	source/imgui_canvas.h
	source/imgui_extra_math.h
	source/imgui_extra_math.inl
	source/imgui_node_editor.cpp
	source/imgui_node_editor.h
	source/imgui_node_editor_api.cpp
	source/imgui_node_editor_internal.h
	source/imgui_node_editor_internal.inl
)

target_include_directories(imgui-node-editor PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/source
)

target_link_libraries(imgui-node-editor PUBLIC
	imgui-docking-layout
)
