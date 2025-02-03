file(GLOB_RECURSE SRC_FILES
	CONFIGURE_DEPENDS
	src/*.cpp
	src/*.c
	src/*.hpp
	src/*.h,
)

file(GLOB_RECURSE HEADER_FILES
	CONFIGURE_DEPENDS
	include/*.h
)

add_library(HitmanAbsolutionSDK SHARED
	${SRC_FILES}
	${HEADER_FILES}
 )

target_include_directories(HitmanAbsolutionSDK PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_SOURCE_DIR}/External/IconFontCppHeaders
	${CMAKE_SOURCE_DIR}/External/mINI/src/mini
)

target_link_libraries(HitmanAbsolutionSDK PUBLIC
	Microsoft::DirectXTK
	imgui-docking-layout
	minhook::minhook
	winmm
)

target_compile_definitions(HitmanAbsolutionSDK PRIVATE
	EXPORTS
)

install(TARGETS HitmanAbsolutionSDK
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)
