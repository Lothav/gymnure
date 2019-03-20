//
// Created by luizorv on 8/27/17.
//

#ifndef OBSIDIAN2D_BUFFER_H
#define OBSIDIAN2D_BUFFER_H

#include "Memory/Memory.h"

struct BufferData
{
    vk::DeviceSize        	size;
    vk::BufferUsageFlags  	usage;
	vk::MemoryPropertyFlags properties;
};

namespace Engine
{
	namespace Memory
	{
		class Buffer
		{

		public:

			vk::Buffer 				buf{};
			vk::DeviceMemory 		mem{};
			uint32_t 				size = 0;

            explicit Buffer(const struct BufferData& buffer_data);

			virtual ~Buffer();

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
