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

#include <vector>
#include <fstream>
#include <cstring>
#include <cassert>
#include <iostream>
#include <chrono>

namespace Engine
{
	namespace Util
	{
		class Util
		{
		public:

			Util() = delete;

			static void initViewport(vk::CommandBuffer cmd_buffer, uint32_t width, uint32_t height);
			static void initScissor(vk::CommandBuffer cmd_buffer, uint32_t width, uint32_t height);
			static std::string physicalDeviceTypeString(vk::PhysicalDeviceType type);
			static vk::ShaderModule loadSPIRVShader(const std::string& filename);
		};
	}
}
#endif //OBSIDIAN2D_CORE_UTIL_H
