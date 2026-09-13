cmake_minimum_required(VERSION 3.8)

project("HitmanAbsolutionSDK" C CXX)

set(CMAKE_CXX_STANDARD 23)
set(GAME_INSTALL_PATH "" CACHE PATH "Path of Hitman Absolution folder.")
set(VCPKG_BUILD_ROOT "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}")

find_package(directxtk CONFIG REQUIRED)
find_package(minhook CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_subdirectory(External/imgui-docking-layout)
add_subdirectory(External/imguizmo)
add_subdirectory(External/imgui-node-editor)

add_subdirectory(HitmanAbsolutionSDK)
add_subdirectory(DirectInputProxy)

set(MODS
	FreeCamera
	StartingLoadout
	Player
	Actors
	Items
	Editor
	Noclip
	HUD
	Camera
)

foreach(MOD IN LISTS MODS)
	add_subdirectory("Mods/${MOD}")
endforeach()

install(
  FILES "${CMAKE_SOURCE_DIR}/HitmanAbsolutionSDK/Resources/HashMap.txt"
  DESTINATION bin
)

if (EXISTS ${GAME_INSTALL_PATH})
	install(FILES "$<TARGET_FILE:DirectInputProxy>" DESTINATION ${GAME_INSTALL_PATH})
	install(FILES "$<TARGET_FILE:HitmanAbsolutionSDK>" DESTINATION ${GAME_INSTALL_PATH})

	foreach(MOD IN LISTS MODS)
		install(FILES "$<TARGET_FILE:${MOD}>" DESTINATION "${GAME_INSTALL_PATH}/mods")
    endforeach()

	install(
    FILES "${CMAKE_SOURCE_DIR}/HitmanAbsolutionSDK/Resources/HashMap.txt"
    DESTINATION "${GAME_INSTALL_PATH}"
  )
endif()

# Collect all licenses from ${VCPKG_BUILD_ROOT}/share/<lib>/copyright into licenses/<lib>.txt
install(CODE "set(VCPKG_BUILD_ROOT \"${VCPKG_BUILD_ROOT}\")")
install(CODE "set(INSTALL_DIR \"${CMAKE_INSTALL_PREFIX}\")")
install(CODE [[
	message(STATUS "Collecting licenses from ${VCPKG_BUILD_ROOT}/share/")
	file(GLOB_RECURSE LICENSES "${VCPKG_BUILD_ROOT}/share/*/copyright")

	foreach(LICENSE IN LISTS LICENSES)
		# Get the name of the library (containing folder).
		get_filename_component(LICENSE_DIR "${LICENSE}" DIRECTORY)
		get_filename_component(LICENSE_DIR "${LICENSE_DIR}" NAME)

		message(STATUS "Installing ${LICENSE} as licenses/${LICENSE_DIR}.txt")
		file(COPY "${LICENSE}" DESTINATION "${INSTALL_DIR}/licenses")
		file(RENAME "${INSTALL_DIR}/licenses/copyright" "${INSTALL_DIR}/licenses/${LICENSE_DIR}.txt")
	endforeach()
]])

# Install licenses from external projects.
install(FILES "External/mINI/LICENSE" DESTINATION licenses RENAME mINI.txt)
install(FILES "External/IconFontCppHeaders/licence.txt" DESTINATION licenses RENAME IconFontCppHeaders.txt)
