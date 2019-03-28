#include "DescriptorSet.h"

namespace Engine
{
    namespace Descriptors
    {
        DescriptorSet::~DescriptorSet()
        {
            auto device = ApplicationData::data->device;

            device.destroyPipelineLayout(pipeline_layout_);
            device.destroyDescriptorSetLayout(desc_layout_);
            device.destroyDescriptorPool(desc_pool_);
        }

        DescriptorSet::DescriptorSet(uint32_t texture_count, uint32_t vertex_uniform_count, uint32_t fragment_uniform_count)
        {
            auto app_data = ApplicationData::data;

            // Create Layouts
            {
                vk::DescriptorSetLayoutBinding l_bind = {};

                uint32_t binding_count = 0;

                for (uint32_t i = 0; i < texture_count; ++i)
                {
                    l_bind.binding 			    = binding_count++;
                    l_bind.descriptorType 	    = vk::DescriptorType::eCombinedImageSampler;
                    l_bind.descriptorCount 	    = 1;
                    l_bind.stageFlags 		    = vk::ShaderStageFlagBits::eFragment;
                    l_bind.pImmutableSamplers   = nullptr;

                    layout_bindings_.push_back(l_bind);
                }

                for (uint32_t i = 0; i < fragment_uniform_count; ++i)
                {
                    l_bind.binding 			    = binding_count++;
                    l_bind.descriptorType 	    = vk::DescriptorType::eUniformBuffer;
                    l_bind.descriptorCount 	    = 1;
                    l_bind.stageFlags 		    = vk::ShaderStageFlagBits::eFragment;
                    l_bind.pImmutableSamplers   = nullptr;

                    layout_bindings_.push_back(l_bind);
                }

                for (uint32_t i = 0; i < vertex_uniform_count; ++i)
                {
                    l_bind.binding 			    = binding_count++;
                    l_bind.descriptorType 	    = vk::DescriptorType::eUniformBuffer;
                    l_bind.descriptorCount 	    = 1;
                    l_bind.stageFlags 		    = vk::ShaderStageFlagBits::eVertex;
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

            // Create Descriptor Set
            {
                std::vector<vk::DescriptorPoolSize> poolSizes = {};

                vk::DescriptorPoolSize poolSize = {};
                poolSize.type = vk::DescriptorType::eUniformBuffer;
                poolSize.descriptorCount = vertex_uniform_count + fragment_uniform_count;
                poolSizes.push_back(poolSize);

                if(texture_count > 0){
                    poolSize.type = vk::DescriptorType::eCombinedImageSampler;
                    poolSize.descriptorCount = texture_count;
                    poolSizes.push_back(poolSize);
                }

                vk::DescriptorPoolCreateInfo descriptor_pool_info = {};
                descriptor_pool_info.maxSets 		= 1;
                descriptor_pool_info.poolSizeCount 	= static_cast<uint32_t>(poolSizes.size());
                descriptor_pool_info.pPoolSizes 	= poolSizes.data();

                desc_pool_ = ApplicationData::data->device.createDescriptorPool(descriptor_pool_info);
            }
        }

        vk::DescriptorSet DescriptorSet::createDescriptorSet() const
        {
            std::vector<vk::DescriptorSet> desc_sets_;

            vk::DescriptorSetAllocateInfo alloc_info_;
            alloc_info_.pNext 				= nullptr;
            alloc_info_.descriptorPool 		= desc_pool_;
            alloc_info_.descriptorSetCount 	= 1;
            alloc_info_.pSetLayouts 		= &desc_layout_;

            return ApplicationData::data->device.allocateDescriptorSets(alloc_info_)[0];
        }

        vk::PipelineLayout DescriptorSet::getPipelineLayout() const
        {
            return pipeline_layout_;
        }
    }
}
