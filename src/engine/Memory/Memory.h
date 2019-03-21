//
// Created by luizorv on 8/25/17.
//

#ifndef OBSIDIAN2D_MEMORY_H
#define OBSIDIAN2D_MEMORY_H

#include <vulkan/vulkan.hpp>
#include <cassert>
#include <cstring>
#include <ApplicationData.hpp>

namespace Engine
{
	namespace Memory
	{
		class Memory
		{
		public:

			static void copyMemory(vk::DeviceMemory device_memory, const void * object, size_t object_size);

			static uint32_t findMemoryType(uint32_t typeBits, const vk::MemoryPropertyFlags& requirements_mask);
		};
	}
}

#endif //OBSIDIAN2D_MEMORY_H
