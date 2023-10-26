cmake_minimum_required(VERSION 3.8)

project("HitmanAbsolutionSDK" CXX)

set(CMAKE_CXX_STANDARD 23)
set(GAME_INSTALL_PATH "" CACHE PATH "Path of Hitman Absolution folder.")
set(VCPKG_BUILD_ROOT "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}")

find_package(directxtk CONFIG REQUIRED)
find_package(minhook CONFIG REQUIRED)

add_subdirectory(External/imgui-docking)

add_subdirectory(HitmanAbsolutionSDK)
add_subdirectory(DirectInputProxy)

set(MODS
	FreeCamera
	StartingOutfitAndWeaponModifier
	Player
	Actors
	Items
)

foreach(MOD IN LISTS MODS)
	add_subdirectory("Mods/${MOD}")
endforeach()

if (EXISTS ${GAME_INSTALL_PATH})
	install(FILES "$<TARGET_FILE:DirectInputProxy>" DESTINATION ${GAME_INSTALL_PATH})
	install(FILES "$<TARGET_FILE:HitmanAbsolutionSDK>" DESTINATION ${GAME_INSTALL_PATH})

	foreach(MOD IN LISTS MODS)
		install(FILES "$<TARGET_FILE:${MOD}>" DESTINATION "${GAME_INSTALL_PATH}/mods")
    endforeach()

	file(TO_CMAKE_PATH "${GAME_INSTALL_PATH}" GAME_INSTALL_PATH_CMAKE)
	install(DIRECTORY "${CMAKE_SOURCE_DIR}/assets/" DESTINATION "${GAME_INSTALL_PATH_CMAKE}/assets")
endif()