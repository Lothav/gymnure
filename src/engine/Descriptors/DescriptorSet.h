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
            vk::PipelineLayout 						    pipeline_layout_    = {};
            vk::DescriptorPool                          desc_pool_          = {};

        public:

            explicit DescriptorSet(uint32_t texture_count, uint32_t vertex_uniform_count, uint32_t fragment_uniform_count);
            ~DescriptorSet();

            void* operator new(std::size_t size)
            {
                return mem::Provider::getMemory(size);
            }

            void operator delete(void* ptr)
            {
                // Do not free memory here!
            }

            vk::DescriptorSet createDescriptorSet() const;

            vk::PipelineLayout getPipelineLayout() const;
        };
    }
}
#endif //OBSIDIAN2D_DESCRIPTORSET_H
