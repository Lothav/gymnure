#include "Memory.h"
#include "Util/Debug.hpp"

namespace Engine
{
    namespace Memory
    {
        void Memory::copyMemory(vk::DeviceMemory device_memory, const void * object, size_t object_size)
        {
            auto device = ApplicationData::data->device;

            void* buffer_address_ = nullptr;
            DEBUG_CALL(buffer_address_ = device.mapMemory(device_memory, 0, object_size, {}));
            memcpy(buffer_address_, object, object_size);
            DEBUG_CALL(device.unmapMemory(device_memory));
        }

        uint32_t Memory::findMemoryType(uint32_t typeBits, const vk::MemoryPropertyFlags& requirements_mask)
        {
            auto app_data = ApplicationData::data;

            // Search memtypes to find first index with those properties
            for (uint32_t i = 0; i < app_data->memory_properties.memoryTypeCount; i++) {
                if ((typeBits & 1) == 1) {
                    // Type is available, does it match user properties?
                    if ((app_data->memory_properties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) {
                        return  i;
                    }
                }
                typeBits >>= 1;
            }
            // No memory types matched, throw exception
            throw "Could not find a suitable memory type!";
        }
    }
}

