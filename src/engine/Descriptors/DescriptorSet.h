//
// Created by tracksale on 9/4/17.
//

#ifndef OBSIDIAN2D_DESCRIPTORSET_H
#define OBSIDIAN2D_DESCRIPTORSET_H

#include <Allocator.hpp>
#include <ApplicationData.hpp>
#include <Util/Debug.hpp>
#include "Descriptors/Camera.h"
#include "Descriptors/Texture.hpp"
#include "Memory/BufferImage.h"

namespace Engine
{
    namespace Descriptors
    {

        class DescriptorSet
        {

        private:

            std::vector<vk::DescriptorSetLayoutBinding> layout_bindings_    = {};
            vk::DescriptorSetLayout 		            desc_layout_        = {};
            vk::PipelineLayout 						    pipeline_layout_    = {};
            vk::DescriptorPool                          desc_pool_          = {};

        public:

            explicit DescriptorSet(uint32_t texture_count, uint32_t vertex_uniform_count, uint32_t fragment_uniform_count);
            ~DescriptorSet();

            std::vector<vk::DescriptorSet> createDescriptorSets(uint32_t objects_count, uint32_t texture_count, uint32_t uniform_count);
            vk::PipelineLayout getPipelineLayout() const;
        };
    }
}
#endif //OBSIDIAN2D_DESCRIPTORSET_H
