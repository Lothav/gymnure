//
// Created by luizorv on 6/10/17.
//

#ifndef OBSIDIAN2D_CORE_UTIL_H
#define OBSIDIAN2D_CORE_UTIL_H
#include "vulkan/vulkan.h"

#if defined(NDEBUG) && defined(__GNUC__)
#define U_ASSERT_ONLY __attribute__((unused))
#else
#define U_ASSERT_ONLY
#endif

#ifdef ASSETS_FOLDER_PATH
#define ASSETS_FOLDER_PATH_STR ASSETS_FOLDER_PATH
#else
#define ASSETS_FOLDER_PATH_STR "."
#endif

#include <xcb/xcb.h>
#include <vector>
#include <fstream>
#include <cstring>
#include <cassert>
#include <iostream>
#include <chrono>

#include <memancpp/Provider.hpp>
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

struct VertexData
{
    float pos[3];
    float uv[2];
    float normal[3];
};

enum WindowEvent
{
	None       = 0,
	Click      = 1,
	Focus      = 2,
	Blur       = 3,
	Resize     = 4,
	Close      = 5,
	ButtonDown = 6,
	ButtonUp   = 7,
	Unknow     = 8,
};

namespace Engine
{
	#ifdef DEBUG
	#define BENCHMARK_FUNCTION(fn, ret)                                                             		 \
	{                                                                                               		 \
	    auto start_cf = std::chrono::high_resolution_clock::now();                                  		 \
	    ret = fn;                                                                                   		 \
	    auto end_cf = std::chrono::high_resolution_clock::now();                                    		 \
	    auto duration_cf = std::chrono::duration<double, std::milli>(end_cf - start_cf).count();    		 \
	    Engine::Debug::logInfo("Took " + std::to_string(duration_cf) + "ms to execute " + std::string(#fn)); \
	}
	#else
	#define BENCHMARK_FUNCTION(fn) fn;
	#endif

	namespace Util
	{
		class Util
		{
		public:

			Util() = delete;

			static void initViewports(VkCommandBuffer cmd_buffer, uint32_t width, uint32_t height);
			static std::string physicalDeviceTypeString(VkPhysicalDeviceType type);
			static void initScissors(VkCommandBuffer cmd_buffer, uint32_t width, uint32_t height);
			static VkShaderModule loadSPIRVShader(const std::string& filename, VkDevice device);
		};
	}
}
#endif //OBSIDIAN2D_CORE_UTIL_H
