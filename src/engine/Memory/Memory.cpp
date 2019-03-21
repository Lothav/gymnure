#include "Memory.h"

namespace Engine
{
    namespace Memory
    {
        void Memory::copyMemory(vk::DeviceMemory device_memory, const void * object, size_t object_size)
        {
            void* _buffer_address = nullptr;

            auto app_data = ApplicationData::data;

            vk::Result res = app_data->device.mapMemory(device_memory, 0, object_size, {}, &_buffer_address);
            assert(res == vk::Result::eSuccess);
            memcpy(_buffer_address, object, object_size);
            vkUnmapMemory(app_data->device, device_memory);
        }

        bool Memory::findMemoryType(uint32_t typeBits, const vk::MemoryPropertyFlags& requirements_mask, uint32_t *typeIndex)
        {
            auto app_data = ApplicationData::data;

            // Search memtypes to find first index with those properties
            for (uint32_t i = 0; i < app_data->memory_properties.memoryTypeCount; i++) {
                if ((typeBits & 1) == 1) {
                    // Type is available, does it match user properties?
                    if ((app_data->memory_properties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) {
                        *typeIndex = i;
                        return true;
                    }
                }
                typeBits >>= 1;
            }
            // No memory types matched, return failure
            return false;
        }
    }
}

