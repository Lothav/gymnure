cmake_minimum_required(VERSION 3.7.0 FATAL_ERROR)
project(Gymnure)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# FIND VULKAN
find_package(Vulkan)
IF (NOT Vulkan_FOUND)
    message(STATUS "Using hardcoded Vulkan lib path")
    set(Vulkan_LIBRARY /home/luiz0tavio/vulkan/1.1.82.1/x86_64/lib/libvulkan.so)
    include_directories(/home/luiz0tavio/vulkan/1.1.82.1/x86_64/include)
    include_directories(/home/luiz0tavio/vulkan/1.1.82.1/x86_64/include/vulkan/)
ENDIF()
message(STATUS "Vulkan lib path: " ${Vulkan_LIBRARY})

# FIND XCB
find_package(XCB REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_XCB_KHR")
message(STATUS "XCB libs paths: " ${XCB_LIBRARIES})

# INCLUD DIRECTORIES
include_directories(include engine/)
include_directories(include external/)

# SOURCE FILES
set(CORE_MAIN_FILE src/main.cpp)
file(GLOB_RECURSE CORE_HEADER_FILES engine/*)
file(GLOB_RECURSE EXTERNAL_HEADER_FILES external/*)

# ADD EXEC AND LINK LIBS
add_executable(core ${CORE_MAIN_FILE} gymnure.h ${CORE_HEADER_FILES} ${EXTERNAL_HEADER_FILES})
target_link_libraries(core ${XCB_LIBRARIES} ${Vulkan_LIBRARY})