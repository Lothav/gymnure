//
// Created by luizorv on 8/27/17.
//

#ifndef OBSIDIAN2D_BUFFER_H
#define OBSIDIAN2D_BUFFER_H

#include <Util/Debug.hpp>
#include "Memory/Memory.h"

struct BufferData
{
    vk::BufferUsageFlags  	usage;
	vk::MemoryPropertyFlags properties;
    size_t                  size;
};

namespace Engine
{
	namespace Memory
	{
		template <class T>
		class Buffer
		{

		private:

			vk::DeviceMemory mem{};
            vk::Buffer buf{};
            size_t size;

		public:

            explicit Buffer(const struct BufferData& buffer_data)
            {
                auto device = ApplicationData::data->device;

                // Backup buffer size
                size = buffer_data.size;

                // Create Buffer
                vk::BufferCreateInfo bufferInfo = {};
                bufferInfo.size 				 = buffer_data.size * sizeof(T);
                bufferInfo.usage 				 = buffer_data.usage;
                bufferInfo.queueFamilyIndexCount = 0;
                bufferInfo.pQueueFamilyIndices 	 = nullptr;
                bufferInfo.sharingMode 			 = vk::SharingMode::eExclusive;
                bufferInfo.pNext 				 = nullptr;
                this->buf = device.createBuffer(bufferInfo);

                // Create Memory
                vk::MemoryRequirements memReq{};
                memReq = device.getBufferMemoryRequirements(this->buf);
                vk::MemoryAllocateInfo allocInfo = {};
                allocInfo.allocationSize  = memReq.size;
                allocInfo.memoryTypeIndex = Memory::findMemoryType(memReq.memoryTypeBits, buffer_data.properties);
                this->mem = device.allocateMemory(allocInfo);

                // Bind Buffer to Memory
                device.bindBufferMemory(this->buf, this->mem, 0);
            };

			virtual ~Buffer()
			{
                auto device = ApplicationData::data->device;
                device.destroyBuffer(this->buf, nullptr);
                device.freeMemory(this->mem, nullptr);
			}

			vk::Buffer getBuffer() const
			{
			    return buf;
			}

			size_t getSize() const
			{
			    return size * sizeof(T);
			}

			void* operator new(std::size_t size)
			{
				return mem::Provider::getMemory(size);
			}

			void operator delete(void* ptr)
			{
				// Do not free memory here!
			}

            void updateBuffer(std::vector<T, std::allocator<T>> data)
            {
                if (data.size() != size)
                    Debug::logError("Invalid data size! Allocate buffer size: " + std::to_string(size) + ".");

                auto device = ApplicationData::data->device;

                size_t data_size = data.size() * sizeof(T);

                void* buffer_address_ = nullptr;
                buffer_address_ = device.mapMemory(this->mem, 0, data_size);
                memcpy(buffer_address_, data.data(), data_size);
                device.unmapMemory(this->mem);
			}
        };
	}
}
#endif //OBSIDIAN2D_BUFFER_H
