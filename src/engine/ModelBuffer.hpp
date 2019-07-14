
#ifndef GYMNURE_MODELBUFFER_HPP
#define GYMNURE_MODELBUFFER_HPP

#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Memory/Buffer.h"

namespace Engine
{
    class ModelBuffer
    {

    private:

        glm::mat4*                                  data_    = nullptr;
        std::unique_ptr<Memory::Buffer<glm::mat4>>  buffer_  = nullptr;
        vk::DescriptorBufferInfo                    buffer_info_ {};

    public:

        explicit ModelBuffer(size_t instances_count)
        {
            size_t dynamicAlignment = Memory::Memory::getDynamicAlignment<glm::mat4>();
            size_t bufferSize = instances_count * dynamicAlignment;

            data_ = (glm::mat4 *) Memory::Memory::alignedAlloc(bufferSize, dynamicAlignment);
            for (uint32_t i = 0; i < instances_count; i ++)
                data_[i] = glm::mat4(1.0f);

            struct BufferData buffer_data = {};
            buffer_data.usage      = vk::BufferUsageFlagBits::eUniformBuffer;
            buffer_data.properties = vk::MemoryPropertyFlagBits::eHostVisible;
            buffer_data.count      = bufferSize / sizeof(glm::mat4);

            buffer_ = std::make_unique<Memory::Buffer<glm::mat4>>(buffer_data);
            buffer_->updateBuffer(data_);

            buffer_info_.offset = 0;
            buffer_info_.range  = VK_WHOLE_SIZE;
            buffer_info_.buffer = buffer_->getBuffer();
        }

        ~ModelBuffer()
        {
            Memory::Memory::alignedFree<glm::mat4>(data_);
        }

        vk::WriteDescriptorSet getWrite(vk::DescriptorSet desc_set, uint32_t dst_bind)
        {
            vk::WriteDescriptorSet write = {};
            write.pNext 			= nullptr;
            write.dstSet 			= desc_set;
            write.descriptorCount 	= 1;
            write.descriptorType 	= vk::DescriptorType::eUniformBufferDynamic;
            write.pBufferInfo 		= &buffer_info_;
            write.dstBinding 		= dst_bind;

            return write;
        }
    };

}

#endif //GYMNURE_MODELBUFFER_HPP
