# Hardcode project included SDL2 2.0.9 Lib path
set(SDL2_SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src/external/SDL2-2.0.9)
set(SDL2_BIN_PATH ${CMAKE_CURRENT_BINARY_DIR}/src/external/SDL2-2.0.9)
set(SDL2_BUILD_COMMAND sdl2BuildCommand)

message(STATUS "Compiling SDL2 from " ${SDL2_SRC_PATH} " in " ${SDL2_BIN_PATH} " ...")
add_subdirectory(${SDL2_SRC_PATH})

# Set variables
set(SDL2_INCLUDE_DIR ${SDL2_SRC_PATH}/include)
set(SDL2_LIBRARY ${SDL2_BIN_PATH}/libSDL2-2.0d.so)
set(SDL2_FOUND TRUE)

# Set the temp variable to INTERNAL so it is not seen in the CMake GUI
set(SDL2_LIBRARY_TEMP "${SDL2_LIBRARY_TEMP}" CACHE INTERNAL "")

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SDL2 REQUIRED_VARS SDL2_LIBRARY SDL2_INCLUDE_DIR)

add_custom_target(${SDL2_BUILD_COMMAND} COMMAND make WORKING_DIRECTORY ${SDL2_BIN_PATH})
