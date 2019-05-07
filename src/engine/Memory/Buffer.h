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
    size_t                  count;
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
            size_t count;

		public:

            explicit Buffer(const struct BufferData& buffer_data)
            {
                auto device = ApplicationData::data->device;

                // Backup buffer size
                count = buffer_data.count;

                // Create Buffer
                vk::BufferCreateInfo bufferInfo = {};
                bufferInfo.size  = buffer_data.count * sizeof(T);
                bufferInfo.usage = buffer_data.usage;
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
			    return count * sizeof(T);
			}

            void updateBuffer(T* data)
            {
                auto device = ApplicationData::data->device;
                size_t data_size = getSize();

                void* buffer_address_ = nullptr;
                buffer_address_ = device.mapMemory(this->mem, 0, data_size);
                memcpy(buffer_address_, data, data_size);
                device.unmapMemory(this->mem);
            }

            void updateBuffer(std::vector<T, std::allocator<T>> data)
            {
                if (data.size() != count)
                    Debug::logErrorAndDie("Invalid data size! Allocate buffer size: " + std::to_string(getSize()) + ".");

                updateBuffer(data.data());
			}
        };
	}
}
#endif //OBSIDIAN2D_BUFFER_H
