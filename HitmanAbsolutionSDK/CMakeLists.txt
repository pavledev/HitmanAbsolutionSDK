file(GLOB_RECURSE SRC_FILES
	CONFIGURE_DEPENDS
	Src/*.cpp
	Src/*.c
	Src/*.hpp
	Src/*.h,
)

file(GLOB_RECURSE HEADER_FILES
	CONFIGURE_DEPENDS
	Include/*.h
)

add_library(HitmanAbsolutionSDK SHARED
	${SRC_FILES}
	${HEADER_FILES}
 )

target_include_directories(HitmanAbsolutionSDK PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/Include
	${CMAKE_SOURCE_DIR}/External/IconFontCppHeaders
	${CMAKE_SOURCE_DIR}/External/mINI/src/mini
)

target_link_libraries(HitmanAbsolutionSDK PUBLIC
	Microsoft::DirectXTK
	imgui-docking-layout
	minhook::minhook
	winmm
	spdlog::spdlog_header_only
)

target_compile_definitions(HitmanAbsolutionSDK PRIVATE
	EXPORTS
)

install(TARGETS HitmanAbsolutionSDK
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)

install(FILES $<TARGET_PDB_FILE:HitmanAbsolutionSDK> DESTINATION bin)
