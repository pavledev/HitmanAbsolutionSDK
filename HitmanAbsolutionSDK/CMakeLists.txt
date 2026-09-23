cmake_minimum_required(VERSION 3.12)

# The current version of the SDK.
set(HitmanAbsolutionSDK_VER_MAJOR 1)
set(HitmanAbsolutionSDK_VER_MINOR 0)
set(HitmanAbsolutionSDK_VER_PATCH 0)

# The current version of the SDK ABI.
# Any change to this is a breaking change.
set(HitmanAbsolutionSDK_ABI_VER 1)

# Generate version-related files.
set(HitmanAbsolutionSDK_RCVERSION "${HitmanAbsolutionSDK_VER_MAJOR},${HitmanAbsolutionSDK_VER_MINOR},${HitmanAbsolutionSDK_VER_PATCH},0")
set(HitmanAbsolutionSDK_RCVERSION_STR "${HitmanAbsolutionSDK_VER_MAJOR}.${HitmanAbsolutionSDK_VER_MINOR}.${HitmanAbsolutionSDK_VER_PATCH}.0")
set(HitmanAbsolutionSDK_VERSION_STR "${HitmanAbsolutionSDK_VER_MAJOR}.${HitmanAbsolutionSDK_VER_MINOR}.${HitmanAbsolutionSDK_VER_PATCH}")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/HitmanAbsolutionSDK.rc.in ${CMAKE_CURRENT_BINARY_DIR}/HitmanAbsolutionSDK.rc)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/ModSDKVersion.h.in ${CMAKE_CURRENT_SOURCE_DIR}/Include/ModSDKVersion.h)

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
  ${CMAKE_CURRENT_BINARY_DIR}/HitmanAbsolutionSDK.rc
 )

target_include_directories(HitmanAbsolutionSDK PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/Include
	${CMAKE_SOURCE_DIR}/External/IconFontCppHeaders
	${CMAKE_SOURCE_DIR}/External/mINI/src/mini
)

target_include_directories(HitmanAbsolutionSDK PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/Src
  ${CMAKE_SOURCE_DIR}/External/mINI/src/mini
)

target_link_libraries(HitmanAbsolutionSDK PUBLIC
	Microsoft::DirectXTK
  imgui::imgui
	minhook::minhook
	winmm
	spdlog::spdlog_header_only
  implot::implot
  winhttp
  semver::semver
  comctl32
)

target_compile_definitions(HitmanAbsolutionSDK PRIVATE
	EXPORTS
)

target_sources(HitmanAbsolutionSDK PRIVATE
  Resources/Resources.rc
)

install(TARGETS HitmanAbsolutionSDK
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)

install(FILES $<TARGET_PDB_FILE:HitmanAbsolutionSDK> DESTINATION bin)
