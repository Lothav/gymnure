cmake_minimum_required(VERSION 3.7.0 FATAL_ERROR)
project(Gymnure)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake-modules")

# ENABLE DEBUG
add_definitions(-DDEBUG)

# SET ASSETS FOLDER PATH
set(ASSETS_FOLDER_PATH ${CMAKE_SOURCE_DIR}/assets)
add_definitions(-DASSETS_FOLDER_PATH="${ASSETS_FOLDER_PATH}")

find_package(Threads REQUIRED)
IF (NOT Threads_FOUND)
    message(FATAL_ERROR "Threads not found!")
ENDIF()

# FIND VULKAN
find_package(Vulkan REQUIRED)
IF (NOT Vulkan_FOUND)
    message(FATAL_ERROR "Vulkan not found!")
ENDIF()
message(STATUS "Vulkan lib path: " ${Vulkan_LIBRARY})
message(STATUS "Vulkan include path: " ${Vulkan_INCLUDE_DIRS})

# FIND SDL2
find_package(SDL2 REQUIRED)
IF (NOT SDL2_FOUND)
    message(FATAL_ERROR "SDL2 not found!")
ENDIF()
message(${SDL2_LIBRARY})

# FIND IMGUI
find_package(IMGUI REQUIRED)
IF (NOT IMGUI_FOUND)
    message(FATAL_ERROR "IMGUI not found!")
ENDIF()

# FIND GLM
find_package(GLM REQUIRED)
IF (NOT GLM_FOUND)
    message(FATAL_ERROR "GLM not found!")
ENDIF()

# FIND MEMANCPP
find_package(MEMANCPP REQUIRED)
IF (NOT MEMANCPP_FOUND)
    message(FATAL_ERROR "MEMANCPP not find!")
ENDIF()

include_directories(src/engine src/external ${Vulkan_INCLUDE_DIRS} ${GLM_INCLUDE_DIR} ${SDL2_INCLUDE_DIR} ${MEMANCPP_INCLUDE_DIR} ${IMGUI_INCLUDE_DIR})

# SOURCE FILES
set(CORE_MAIN_FILE src/main.cpp)
file(GLOB_RECURSE CORE_HEADER_FILES src/engine/*)

# ADD EXEC AND LINK LIBS
add_executable(gymnure src/external/tinyobjloader/tiny_obj_loader.h src/external/stb/stb_image.h  ${GLM_FILES} ${IMGUI_FILES} ${MEMANCPP_FILES} ${CORE_HEADER_FILES} ${CORE_MAIN_FILE})
# LINK LIBRARIES
target_link_libraries(gymnure ${CMAKE_THREAD_LIBS_INIT} ${SDL2_LIBRARY} ${Vulkan_LIBRARY})
# BUILD SDL2 DEPENDENCY
#add_dependencies(gymnure ${SDL2_BUILD_COMMAND})
# COMPILE SHADERS BEFORE RUN

find_program(glslangValidator NAMES glslangValidator HINTS /opt/glslang/bin /opt/glslangValidator/bin)

if(glslangValidator)
    message(STATUS "Found glslangValidator: ${glslangValidator}")

    # Define a macro to compile a GLSL shader to SPIR-V
    macro(add_spirv shadername stage)
        set(SPIRV_OUTPUT "${PROJECT_BINARY_DIR}/spirv/${shadername}.spv")
        set(SPIRV_INPUT "${PROJECT_SOURCE_DIR}/assets/shaders/${shadername}.glsl")
        message(STATUS "Compiling ${SPIRV_INPUT} to ${SPIRV_OUTPUT}")

        add_custom_command(
            OUTPUT ${SPIRV_OUTPUT}
            COMMAND ${glslangValidator} -V -S ${stage} -o ${SPIRV_OUTPUT} ${SPIRV_INPUT}
            DEPENDS ${SPIRV_INPUT}
        )
        set(spirvDependencies ${spirvDependencies} ${SPIRV_OUTPUT})
    endmacro()

    # Compile Shaders
    add_spirv(phong_fs frag)
    add_spirv(phong_vs vert)

    message(STATUS "SPIR-V dependencies: ${spirvDependencies}")
    add_custom_target(spirv DEPENDS ${spirvDependencies})
    add_dependencies(gymnure spirv)
    install(DIRECTORY ${PROJECT_BINARY_DIR}/spirv DESTINATION ${PROJECT_SOURCE_DIR}/assets/shaders)
else()
    message(SEND_ERROR "glslangValidator not found - will not compile GLSL shaders to SPIR-V during build (https://github.com/KhronosGroup/glslang/releases/tag/master-tot)")
endif()

if(WIN32 AND SDL2_DLL)
    # After build, copy SDL2.dll to binary folder to make sure that it will be find.
    add_custom_command(TARGET gymnure POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SDL2_DLL} ${PROJECT_BINARY_DIR} )
endif()
