//
// Created by luizorv on 8/25/17.
//

#ifndef OBSIDIAN2D_MEMORY_H
#define OBSIDIAN2D_MEMORY_H

#include <vulkan/vulkan.hpp>
#include <cassert>
#include <cstring>
#include <ApplicationData.hpp>
#include <glm/glm.hpp>

namespace Engine
{
	namespace Memory
	{
		class Memory
		{
		public:

			static uint32_t findMemoryType(uint32_t typeBits, const vk::MemoryPropertyFlags& requirements_mask);

			static void* alignedAlloc(size_t size, size_t alignment);

			static size_t getDynamicAlignment()
			{
				// Calculate required alignment based on minimum device offset alignment
				size_t minUboAlignment = ApplicationData::data->gpu.getProperties().limits.minUniformBufferOffsetAlignment;
				size_t dynamicAlignment = sizeof(glm::mat4);
				if (minUboAlignment > 0) {
					dynamicAlignment = (dynamicAlignment + minUboAlignment - 1) & ~(minUboAlignment - 1);
				}
				return dynamicAlignment;
			}
		};
	}
}

#endif //OBSIDIAN2D_MEMORY_H
