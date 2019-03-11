cmake_minimum_required(VERSION 3.7.0 FATAL_ERROR)
project(Gymnure)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake-modules")

# ENABLE DEBUG
add_definitions(-DDEBUG)

# FIND VULKAN
find_package(Vulkan REQUIRED)
IF (NOT Vulkan_FOUND)
    message(FATAL_ERROR "Vulkan not find!")
ENDIF()
message(STATUS "Vulkan lib path: " ${Vulkan_LIBRARY})

# FIND SDL2
find_package(SDL2 REQUIRED)
IF (NOT SDL2_FOUND)
    message(FATAL_ERROR "SDL2 not find!")
ENDIF()

# FIND IMGUI
find_package(IMGUI REQUIRED)
IF (NOT IMGUI_FOUND)
    message(FATAL_ERROR "IMGUI not find!")
ENDIF()

# FIND IMGUI
find_package(MEMANCPP REQUIRED)
IF (NOT MEMANCPP_FOUND)
    message(FATAL_ERROR "MEMANCPP not find!")
ENDIF()

# INCLUDE DIRECTORIES
include_directories(src/engine/ src/external ${SDL2_INCLUDE_DIR} ${MEMANCPP_INCLUDE_DIR} ${IMGUI_INCLUDE_DIR})

# SOURCE FILES
set(CORE_MAIN_FILE src/main.cpp)
file(GLOB_RECURSE CORE_HEADER_FILES src/engine/*)

# ADD EXEC AND LINK LIBS
add_executable(gymnure src/external/tinyobjloader/tiny_obj_loader.h src/external/stb/stb_image.h ${IMGUI_FILES} ${MEMANCPP_FILES} ${CORE_MAIN_FILE} ${CORE_HEADER_FILES})
# LINK LIBRARIES
target_link_libraries(gymnure ${SDL2_LIBRARY} ${Vulkan_LIBRARY})
# BUILD SDL2 DEPENDENCY
add_dependencies(gymnure ${SDL2_BUILD_COMMAND})
