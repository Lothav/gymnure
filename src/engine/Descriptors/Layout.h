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
        struct LayoutData
        {
            bool has_model_matrix           = true;
            bool has_view_projection_matrix = true;

            uint32_t vertex_uniform_count   = 0;
            uint32_t vertex_texture_count   = 0;

            uint32_t fragment_texture_count = 0;
            uint32_t fragment_uniform_count = 0;
        };

        class Layout
        {

        private:

            std::vector<vk::DescriptorSetLayoutBinding> layout_bindings_    = {};
            vk::DescriptorSetLayout 		            desc_layout_        = {};
            vk::PipelineLayout 						    pipeline_layout_    = {};
            vk::DescriptorPool                          desc_pool_          = {};

            std::shared_ptr<LayoutData>                 ds_data_            = {};

        public:

            explicit Layout(const LayoutData& layout_data);
            ~Layout();

            std::vector<vk::DescriptorSet> createDescriptorSets(uint32_t objects_count);
            vk::PipelineLayout getPipelineLayout() const;
            std::shared_ptr<LayoutData> getLayoutData() const;
        };
    }
}
#endif //OBSIDIAN2D_DESCRIPTORSET_H
