//
// Created by tracksale on 9/4/17.
//

#ifndef OBSIDIAN2D_DESCRIPTORSET_H
#define OBSIDIAN2D_DESCRIPTORSET_H

#include <Provider.hpp>
#include <Allocator.hpp>
#include <ApplicationData.hpp>
#include <Util/Debug.hpp>
#include "Descriptors/UniformBuffer.h"
#include "Descriptors/Texture.hpp"
#include "Memory/BufferImage.h"

namespace Engine
{
    namespace Descriptors
    {

        class DescriptorSet
        {

        private:

            std::vector<vk::DescriptorSetLayoutBinding,
                mem::StdAllocator<
                    vk::DescriptorSetLayoutBinding>>    layout_bindings_    = {};
            vk::DescriptorSetLayout 		            desc_layout_        = {};
            vk::PipelineLayout 						    pipeline_layout_    = nullptr;
            Descriptors::UniformBuffer*                 uniform_buffer_     = nullptr;

            uint32_t                                    texture_count_      = 0;

        public:

            explicit DescriptorSet(uint32_t texture_count) : texture_count_(texture_count) {}
            ~DescriptorSet();

            void* operator new(std::size_t size)
            {
                return mem::Provider::getMemory(size);
            }

            void operator delete(void* ptr)
            {
                // Do not free memory here!
            }

            void create();
            void updateDescriptorSet(Texture* texture, vk::DescriptorSet desc_set);
            vk::DescriptorPool createDescriptorPool();
            vk::DescriptorSet createDescriptorSet(vk::DescriptorPool desc_pool);
            vk::PipelineLayout getPipelineLayout() const;
            UniformBuffer* getUniformBuffer() const;
        };
    }
}
#endif //OBSIDIAN2D_DESCRIPTORSET_H
