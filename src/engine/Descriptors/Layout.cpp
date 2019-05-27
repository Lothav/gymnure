 #include "Layout.h"

namespace Engine
{
    namespace Descriptors
    {
        Layout::~Layout()
        {
            auto device = ApplicationData::data->device;

            device.destroyPipelineLayout(pipeline_layout_);
            device.destroyDescriptorSetLayout(desc_layout_);
            device.destroyDescriptorPool(desc_pool_);
        }

        Layout::Layout(const LayoutData& ds_data) : ds_data_(ds_data)
        {
            auto app_data = ApplicationData::data;

            uint32_t binding_count = 0;

            // Create Layouts
            vk::DescriptorSetLayoutBinding l_bind = {};

            if(ds_data.has_model_matrix) {
                // Model Matrix
                l_bind.binding 			    = binding_count++;
                l_bind.descriptorType 	    = vk::DescriptorType::eUniformBufferDynamic;
                l_bind.descriptorCount 	    = 1;
                l_bind.stageFlags 		    = vk::ShaderStageFlagBits::eVertex;
                l_bind.pImmutableSamplers   = nullptr;
                layout_bindings_.push_back(l_bind);
            }

            if(ds_data.has_view_projection_matrix) {
                // View-Projection Matrix
                l_bind.binding 			    = binding_count++;
                l_bind.descriptorType 	    = vk::DescriptorType::eUniformBuffer;
                l_bind.descriptorCount 	    = 1;
                l_bind.stageFlags 		    = vk::ShaderStageFlagBits::eVertex;
                l_bind.pImmutableSamplers   = nullptr;
                layout_bindings_.push_back(l_bind);
            }

            for (uint32_t i = 0; i < ds_data.vertex_uniform_count; ++i)
            {
                l_bind.binding 			    = binding_count++;
                l_bind.descriptorType 	    = vk::DescriptorType::eUniformBuffer;
                l_bind.descriptorCount 	    = 1;
                l_bind.stageFlags 		    = vk::ShaderStageFlagBits::eVertex;
                l_bind.pImmutableSamplers   = nullptr;

                layout_bindings_.push_back(l_bind);
            }

            for (uint32_t i = 0; i < ds_data.vertex_texture_count; ++i)
            {
                l_bind.binding 			    = binding_count++;
                l_bind.descriptorType 	    = vk::DescriptorType::eCombinedImageSampler;
                l_bind.descriptorCount 	    = 1;
                l_bind.stageFlags 		    = vk::ShaderStageFlagBits::eVertex;
                l_bind.pImmutableSamplers   = nullptr;

                layout_bindings_.push_back(l_bind);
            }

            for (uint32_t i = 0; i < ds_data.fragment_uniform_count; ++i)
            {
                l_bind.binding 			    = binding_count++;
                l_bind.descriptorType 	    = vk::DescriptorType::eUniformBuffer;
                l_bind.descriptorCount 	    = 1;
                l_bind.stageFlags 		    = vk::ShaderStageFlagBits::eFragment;
                l_bind.pImmutableSamplers   = nullptr;

                layout_bindings_.push_back(l_bind);
            }

            for (uint32_t i = 0; i < ds_data.fragment_texture_count; ++i)
            {
                l_bind.binding 			    = binding_count++;
                l_bind.descriptorType 	    = vk::DescriptorType::eCombinedImageSampler;
                l_bind.descriptorCount 	    = 1;
                l_bind.stageFlags 		    = vk::ShaderStageFlagBits::eFragment;
                l_bind.pImmutableSamplers   = nullptr;

                layout_bindings_.push_back(l_bind);
            }

            // Set Descriptor Layouts
            vk::DescriptorSetLayoutCreateInfo descriptor_layout_ = {};
            descriptor_layout_.pNext 						 = nullptr;
            descriptor_layout_.bindingCount 				 = static_cast<uint32_t>(layout_bindings_.size());
            descriptor_layout_.pBindings 					 = layout_bindings_.data();

            desc_layout_ = app_data->device.createDescriptorSetLayout(descriptor_layout_);

            // Set Pipeline Layout
            vk::PipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {};
            pPipelineLayoutCreateInfo.pNext                  = nullptr;
            pPipelineLayoutCreateInfo.pushConstantRangeCount = 0;
            pPipelineLayoutCreateInfo.pPushConstantRanges    = nullptr;
            pPipelineLayoutCreateInfo.setLayoutCount         = 1;
            pPipelineLayoutCreateInfo.pSetLayouts            = &desc_layout_;

            pipeline_layout_ = app_data->device.createPipelineLayout(pPipelineLayoutCreateInfo);
        }

        std::vector<vk::DescriptorSet> Layout::createDescriptorSets(uint32_t objects_count)
        {
            // Create Descriptor Set
            {
                std::vector<vk::DescriptorPoolSize> poolSizes = {};

                vk::DescriptorPoolSize poolSize = {};

                if(ds_data_.has_model_matrix){
                    poolSize.type = vk::DescriptorType::eUniformBufferDynamic;
                    poolSize.descriptorCount = objects_count; // M matrix.
                    poolSizes.push_back(poolSize);
                }

                uint32_t uniform_count = ds_data_.vertex_uniform_count + ds_data_.fragment_uniform_count;
                if(uniform_count > 0){
                    uint32_t vp_mat_count = ds_data_.has_view_projection_matrix ? 1 : 0;
                    poolSize.type = vk::DescriptorType::eUniformBuffer;
                    poolSize.descriptorCount = objects_count * (vp_mat_count + uniform_count); // VP matrix + uniform_count.
                    poolSizes.push_back(poolSize);
                }

                uint32_t texture_count = ds_data_.vertex_texture_count + ds_data_.fragment_texture_count;
                if(texture_count > 0) {
                    poolSize.type = vk::DescriptorType::eCombinedImageSampler;
                    poolSize.descriptorCount = objects_count * texture_count;
                    poolSizes.push_back(poolSize);
                }

                vk::DescriptorPoolCreateInfo descriptor_pool_info = {};
                descriptor_pool_info.maxSets 		= objects_count;
                descriptor_pool_info.poolSizeCount 	= static_cast<uint32_t>(poolSizes.size());
                descriptor_pool_info.pPoolSizes 	= poolSizes.data();

                desc_pool_ = ApplicationData::data->device.createDescriptorPool(descriptor_pool_info);
            }

            // Use same Layout for each Descriptor Set
            std::vector<vk::DescriptorSetLayout> layouts = {};
            for (int i = 0; i < objects_count; ++i)
                layouts.push_back(desc_layout_);

            vk::DescriptorSetAllocateInfo alloc_info_;
            alloc_info_.pNext 				= nullptr;
            alloc_info_.descriptorPool 		= desc_pool_;
            alloc_info_.descriptorSetCount 	= objects_count;
            alloc_info_.pSetLayouts 		= layouts.data();

            return ApplicationData::data->device.allocateDescriptorSets(alloc_info_);
        }

        vk::PipelineLayout Layout::getPipelineLayout() const
        {
            return pipeline_layout_;
        }
    }
}
