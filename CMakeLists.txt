cmake_minimum_required(VERSION 3.31) #3.6

project(Chessboard)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_PROJECT_HIDDEN TRUE)

set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/libs" ${CMAKE_PREFIX_PATH})

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

include_directories(include res)

add_executable(${PROJECT_NAME} 
    src/Main.cpp src/board.cpp src/piece.cpp src/graphics.cpp
    include/board.hpp include/piece.hpp include/graphics.hpp)

target_link_libraries(${PROJECT_NAME}  ${SDL2_LIBRARIES} SDL2_image::SDL2_image )

set(_sdl_arch_subdir "x86")
if(CMAKE_SIZEOF_VOID_P STREQUAL "8")
    set(_sdl_arch_subdir "x64")
endif()

get_filename_component(SDL2_IMAGE_LIB_DIR "${SDL2_image_DIR}" DIRECTORY)

set(SDL2_DLL_PATH "${SDL2_BINDIR}/SDL2.dll")
set(SDL2_IMAGE_DLL_PATH "${SDL2_IMAGE_LIB_DIR}/lib/${_sdl_arch_subdir}/SDL2_image.dll")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SDL2_DLL_PATH}
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SDL2_IMAGE_DLL_PATH}
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

# OPTIONAL - Copying game resources to build target
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        "${CMAKE_SOURCE_DIR}/res/"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/res"
)

SET_PROPERTY(TARGET ${PROJECT_NAME}
    PROPERTY VS_DEBUGGER_WORKING_DIRECTORY
    ${PROJECT_SOURCE_DIR})

SET_PROPERTY(TARGET ${PROJECT_NAME}
    PROPERTY VS_RELEASE_WORKING_DIRECTORY
    ${PROJECT_SOURCE_DIR})

SET_PROPERTY(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
    PROPERTY VS_STARTUP_PROJECT 
    ${PROJECT_NAME})

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")