//
// Created by luizorv on 8/27/17.
//

#ifndef OBSIDIAN2D_BUFFER_H
#define OBSIDIAN2D_BUFFER_H

#include "Memory/Memory.h"

struct BufferData {
    VkPhysicalDevice    physicalDevice;
    VkDevice            device;
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
			VkDevice _instance_device;

		public:

			VkBuffer 									buf;
			VkDeviceMemory 								mem;
			VkDescriptorBufferInfo 						buffer_info;

			virtual ~Buffer()
			{
				vkDestroyBuffer(_instance_device, buf, nullptr);
				vkFreeMemory(_instance_device, mem, nullptr);
			}

			Buffer(struct BufferData buffer_data)
			{
				this->_instance_device = buffer_data.device;

				VkResult res;
				bool pass;

				VkBufferCreateInfo bufferInfo = {};

				bufferInfo.sType 						= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
				bufferInfo.size 						= buffer_data.size;
				bufferInfo.usage 						= buffer_data.usage;
				bufferInfo.queueFamilyIndexCount		= 0;
				bufferInfo.pQueueFamilyIndices 			= nullptr;
				bufferInfo.sharingMode 					= VK_SHARING_MODE_EXCLUSIVE;
				bufferInfo.flags 						= 0;
				bufferInfo.pNext 						= nullptr;

				res = vkCreateBuffer(buffer_data.device, &bufferInfo, nullptr, &this->buf);
				assert(res == VK_SUCCESS);

				VkMemoryRequirements memRequirements;
				vkGetBufferMemoryRequirements(buffer_data.device, this->buf, &memRequirements);

				VkPhysicalDeviceMemoryProperties memProperties;
				vkGetPhysicalDeviceMemoryProperties(buffer_data.physicalDevice, &memProperties);

				VkMemoryAllocateInfo allocInfo = {};
				allocInfo.sType 						= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				allocInfo.allocationSize 				= memRequirements.size;
				allocInfo.pNext 						= nullptr;

				pass = Memory::findMemoryType (
						memProperties,
						memRequirements.memoryTypeBits,
                        buffer_data.properties,
						&allocInfo.memoryTypeIndex);

				assert(pass);

				res = vkAllocateMemory(buffer_data.device, &allocInfo, nullptr, &this->mem);
				assert(res == VK_SUCCESS);

				vkBindBufferMemory(buffer_data.device, this->buf, this->mem, 0);

				this->buffer_info.range 				= buffer_data.size;
				this->buffer_info.offset 				= 0;
				this->buffer_info.buffer 				= this->buf;
			}
        };
	}
}
#endif //OBSIDIAN2D_BUFFER_H
