//
// Created by luizorv on 8/27/17.
//

#ifndef OBSIDIAN2D_BUFFER_H
#define OBSIDIAN2D_BUFFER_H

#include "Memory/Memory.h"

struct BufferData
{
    VkDeviceSize        size;
    VkBufferUsageFlags  usage;
    VkFlags             properties;
};

namespace Engine
{
	namespace Memory
	{
		class Buffer
		{

		public:

			VkBuffer 				buf{};
			VkDeviceMemory 			mem{};
			uint32_t 				size = 0;

            explicit Buffer(const struct BufferData& buffer_data)
			{
				auto app_data = ApplicationData::data;
				VkResult res;

				VkBufferCreateInfo bufferInfo = {};
				bufferInfo.sType 				 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
				bufferInfo.size 				 = buffer_data.size;
				bufferInfo.usage 				 = buffer_data.usage;
				bufferInfo.queueFamilyIndexCount = 0;
				bufferInfo.pQueueFamilyIndices 	 = nullptr;
				bufferInfo.sharingMode 			 = VK_SHARING_MODE_EXCLUSIVE;
				bufferInfo.flags 				 = 0;
				bufferInfo.pNext 				 = nullptr;

				res = vkCreateBuffer(app_data->device, &bufferInfo, nullptr, &this->buf);
				assert(res == VK_SUCCESS);

				size = buffer_data.size;

				VkMemoryRequirements memRequirements;
				vkGetBufferMemoryRequirements(app_data->device, this->buf, &memRequirements);

				VkMemoryAllocateInfo allocInfo = {};
				allocInfo.sType 				 = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				allocInfo.allocationSize 		 = memRequirements.size;
				allocInfo.pNext 				 = nullptr;

				auto pass = Memory::findMemoryType(memRequirements.memoryTypeBits, buffer_data.properties, &allocInfo.memoryTypeIndex);
				assert(pass);

				res = vkAllocateMemory(app_data->device, &allocInfo, nullptr, &this->mem);
				assert(res == VK_SUCCESS);

				res = vkBindBufferMemory(app_data->device, this->buf, this->mem, 0);
				assert(res == VK_SUCCESS);
			}

			virtual ~Buffer()
			{
				auto device = ApplicationData::data->device;
				vkDestroyBuffer(device, this->buf, nullptr);
				vkFreeMemory(device, this->mem, nullptr);
			}

			void* operator new(std::size_t size)
			{
				return mem::Provider::getMemory(size);
			}

			void operator delete(void* ptr)
			{
				// Do not free memory here!
			}
        };
	}
}
#endif //OBSIDIAN2D_BUFFER_H
