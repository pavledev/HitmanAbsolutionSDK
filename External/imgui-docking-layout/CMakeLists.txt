cmake_minimum_required(VERSION 3.8)

add_library(imgui-docking-layout STATIC
	source/imgui.cpp
	source/imgui.h
	source/imgui_draw.cpp
	source/imgui_internal.h
	source/imgui_tables.cpp
	source/imgui_widgets.cpp
	source/imstb_rectpack.h
	source/imstb_textedit.h
	source/imstb_truetype.h
	source/imgui_demo.cpp
    source/backends/imgui_impl_dx11.cpp
    source/backends/imgui_impl_dx11.h
    source/backends/imgui_impl_win32.cpp
    source/backends/imgui_impl_win32.h
    source/misc/cpp/imgui_stdlib.cpp
    source/misc/cpp/imgui_stdlib.h
)

target_include_directories(imgui-docking-layout PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/source
)

set_target_properties(imgui-docking-layout PROPERTIES
	PUBLIC_HEADER "source/imgui.h;source/imconfig.h"
)

install(TARGETS imgui-docking-layout
	LIBRARY DESTINATION lib
	PUBLIC_HEADER DESTINATION include
)
