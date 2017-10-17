//
// Created by luizorv on 8/25/17.
//

#ifndef OBSIDIAN2D_MEMORY_H
#define OBSIDIAN2D_MEMORY_H

#include <vulkan/vulkan.h>
#include <cassert>
#include <cstring>

namespace Engine
{
	namespace Memory
	{
		class Memory
		{
		public:

			static void copyMemory (
					VkDevice device,
					VkDeviceMemory device_memory,
					const void * object,
					size_t object_size)
			{
				VkResult res;
				void* _buffer_address = nullptr;

				res = vkMapMemory(device, device_memory, 0, object_size, 0, &_buffer_address);
				assert(res == VK_SUCCESS);
				memcpy(_buffer_address, object, object_size);
				vkUnmapMemory(device, device_memory);
			}

			static bool findMemoryType(
					VkPhysicalDeviceMemoryProperties memory_properties,
					uint32_t typeBits,
					VkFlags requirements_mask,
					uint32_t *typeIndex)
			{
				// Search memtypes to find first index with those properties
				for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++) {
					if ((typeBits & 1) == 1) {
						// Type is available, does it match user properties?
						if ((memory_properties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) {
							*typeIndex = i;
							return true;
						}
					}
					typeBits >>= 1;
				}
				// No memory types matched, return failure
				return false;
			}
		};
	}
}

#endif //OBSIDIAN2D_MEMORY_H
