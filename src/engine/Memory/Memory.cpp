#include "Memory.h"
#include "Util/Debug.hpp"

namespace Engine
{
    namespace Memory
    {
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

        // Wrapper functions for aligned memory allocation
        // There is currently no standard for this in C++ that works across all platforms and vendors, so we abstract this
        void* Memory::alignedAlloc(size_t size, size_t alignment)
        {
            void *data = nullptr;
        #if defined(_MSC_VER) || defined(__MINGW32__)
            data = _aligned_malloc(size, alignment);
        #else
            int res = posix_memalign(&data, alignment, size);
            if (res != 0)
                data = nullptr;
        #endif
            return data;
        }

    }
}

