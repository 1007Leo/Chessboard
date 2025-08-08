cmake_minimum_required(VERSION 3.31) #3.6

project(Chessboard)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_PROJECT_HIDDEN TRUE)

# to be able to find boost
if(POLICY CMP0167)
   cmake_policy(SET CMP0167 OLD)
endif()

if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0601)
    # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
else()
endif()

set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/libs" ${CMAKE_PREFIX_PATH})

set(Boost_USE_STATIC_LIBS ON)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem)

file(GLOB_RECURSE HEADER_FILES "${CMAKE_SOURCE_DIR}/include/*.hpp")
file(GLOB_RECURSE SRC_FILES "${CMAKE_SOURCE_DIR}/src/*.cpp")
add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS} include)
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} ${SDL2_LIBRARIES} SDL2_image::SDL2_image SDL2_ttf::SDL2_ttf)

set(_sdl_arch_subdir "x86")
if(CMAKE_SIZEOF_VOID_P STREQUAL "8")
    set(_sdl_arch_subdir "x64")
endif()

get_filename_component(SDL2_IMAGE_LIB_DIR "${SDL2_image_DIR}" DIRECTORY)
get_filename_component(SDL2_TTF_LIB_DIR "${SDL2_ttf_DIR}" DIRECTORY)

set(SDL2_DLL_PATH "${SDL2_BINDIR}/SDL2.dll")
set(SDL2_IMAGE_DLL_PATH "${SDL2_IMAGE_LIB_DIR}/lib/${_sdl_arch_subdir}/SDL2_image.dll")
set(SDL2_TTF_DLL_PATH "${SDL2_TTF_LIB_DIR}/lib/${_sdl_arch_subdir}/SDL2_ttf.dll")

# Set debug define
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:DEBUG>)

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

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SDL2_TTF_DLL_PATH}
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

# OPTIONAL - Copy game resources to build target
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
