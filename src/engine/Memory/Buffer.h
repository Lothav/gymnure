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

        protected:

            BufferData buffer_data_{};

		public:

			VkBuffer 				buf{};
			VkDeviceMemory 			mem{};
			VkDescriptorBufferInfo 	buffer_info{};

			virtual ~Buffer()
			{
				auto app_data = ApplicationData::data;
                vkDestroyBuffer(app_data->device, this->buf, nullptr);
				vkFreeMemory(app_data->device, this->mem, nullptr);
            }

            explicit Buffer(const struct BufferData& buffer_data)
			{
				auto app_data = ApplicationData::data;

				VkResult res;
				bool pass;

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

				VkMemoryRequirements memRequirements;
				vkGetBufferMemoryRequirements(app_data->device, this->buf, &memRequirements);

				VkPhysicalDeviceMemoryProperties memProperties;
				vkGetPhysicalDeviceMemoryProperties(app_data->gpu, &memProperties);

				VkMemoryAllocateInfo allocInfo = {};
				allocInfo.sType 						= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				allocInfo.allocationSize 				= memRequirements.size;
				allocInfo.pNext 						= nullptr;

				pass = Memory::findMemoryType(memRequirements.memoryTypeBits, buffer_data.properties, &allocInfo.memoryTypeIndex);
				assert(pass);

				res = vkAllocateMemory(app_data->device, &allocInfo, nullptr, &this->mem);
				assert(res == VK_SUCCESS);

				vkBindBufferMemory(app_data->device, this->buf, this->mem, 0);

				this->buffer_info.range  = buffer_data.size;
				this->buffer_info.offset = 0;
				this->buffer_info.buffer = this->buf;
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
