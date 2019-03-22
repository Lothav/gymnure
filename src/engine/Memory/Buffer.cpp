#include "Buffer.h"

namespace Engine
{
    namespace Memory
    {
        Buffer::Buffer(const struct BufferData& buffer_data)
        {
            auto app_data = ApplicationData::data;

            vk::BufferCreateInfo bufferInfo = {};
            bufferInfo.size 				 = buffer_data.size;
            bufferInfo.usage 				 = buffer_data.usage;
            bufferInfo.queueFamilyIndexCount = 0;
            bufferInfo.pQueueFamilyIndices 	 = nullptr;
            bufferInfo.sharingMode 			 = vk::SharingMode::eExclusive;
            bufferInfo.pNext 				 = nullptr;

            this->buf = app_data->device.createBuffer(bufferInfo);

            size = static_cast<uint32_t>(buffer_data.size);

            vk::MemoryRequirements memRequirements{};
            app_data->device.getBufferMemoryRequirements(this->buf, &memRequirements);

            vk::MemoryAllocateInfo allocInfo = {};
            allocInfo.allocationSize 		 = memRequirements.size;
            allocInfo.pNext 				 = nullptr;

            allocInfo.memoryTypeIndex = Memory::findMemoryType(memRequirements.memoryTypeBits, buffer_data.properties);

            this->mem = app_data->device.allocateMemory(allocInfo);

            app_data->device.bindBufferMemory(this->buf, this->mem, 0);
        }

        Buffer::~Buffer()
        {
            auto device = ApplicationData::data->device;
            device.destroyBuffer(this->buf, nullptr);
            device.freeMemory(this->mem, nullptr);
        }
    }
}
