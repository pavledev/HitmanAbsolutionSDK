cmake_minimum_required(VERSION 3.8)

add_library(HitmanAbsolutionSDK SHARED IMPORTED GLOBAL)

set(HMA_BUILD_TYPE "release")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(HMA_BUILD_TYPE "debug")
endif()

if(NOT DEFINED HMA_SDK_DIST_DIR)
    set(HMA_SDK_DIST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${HMA_BUILD_TYPE}")
endif()

if(NOT DEFINED HMA_SDK_INCLUDE_DIR)
    set(HMA_SDK_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
endif()

set_target_properties(HitmanAbsolutionSDK PROPERTIES
    IMPORTED_LOCATION "${HMA_SDK_DIST_DIR}/bin/HitmanAbsolutionSDK.dll"
    IMPORTED_IMPLIB "${HMA_SDK_DIST_DIR}/lib/HitmanAbsolutionSDK.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${HMA_SDK_INCLUDE_DIR}"
    INTERFACE_LINK_DIRECTORIES "${HMA_SDK_DIST_DIR}/lib"
)

target_link_libraries(HitmanAbsolutionSDK INTERFACE
    imgui
)