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
        }

        void DescriptorSet::create()
        {
            auto app_data = ApplicationData::data;

            // Set Layout Bindings
            layout_bindings_.resize(1 + (texture_count_ > 0 ? 1 : 0));

            layout_bindings_[0].binding 					 = 0;
            layout_bindings_[0].descriptorType 				 = vk::DescriptorType::eUniformBuffer;
            layout_bindings_[0].descriptorCount 			 = 1;
            layout_bindings_[0].stageFlags 					 = vk::ShaderStageFlagBits::eVertex;
            layout_bindings_[0].pImmutableSamplers			 = nullptr;

            if (texture_count_ > 0) {
                layout_bindings_[1].binding 				 = 1;
                layout_bindings_[1].descriptorType 			 = vk::DescriptorType::eCombinedImageSampler;
                layout_bindings_[1].descriptorCount 		 = 1;
                layout_bindings_[1].stageFlags 				 = vk::ShaderStageFlagBits::eFragment;
                layout_bindings_[1].pImmutableSamplers 		 = nullptr;
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

        vk::DescriptorPool DescriptorSet::createDescriptorPool()
        {
            std::vector<vk::DescriptorPoolSize> poolSizes = {};

            poolSizes.resize(1 + (texture_count_ > 0 ? 1 : 0));
            poolSizes[0].type = vk::DescriptorType::eUniformBuffer;
            poolSizes[0].descriptorCount = 1;

            if(texture_count_ > 0){
                poolSizes[1].type = vk::DescriptorType::eCombinedImageSampler;
                poolSizes[1].descriptorCount = 1;
            }

            vk::DescriptorPoolCreateInfo descriptor_pool_info = {};
            descriptor_pool_info.maxSets 		= 1;
            descriptor_pool_info.poolSizeCount 	= static_cast<uint32_t>(poolSizes.size());
            descriptor_pool_info.pPoolSizes 	= poolSizes.data();

            return ApplicationData::data->device.createDescriptorPool(descriptor_pool_info);
        }

        vk::DescriptorSet DescriptorSet::createDescriptorSet(vk::DescriptorPool desc_pool)
        {
            std::vector<vk::DescriptorSet> desc_sets_;

            vk::DescriptorSetAllocateInfo alloc_info_;
            alloc_info_.pNext 				= nullptr;
            alloc_info_.descriptorPool 		= desc_pool;
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
