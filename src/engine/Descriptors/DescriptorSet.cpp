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

            delete uniform_buffer_;
        }

        void DescriptorSet::create()
        {
            auto device = ApplicationData::data->device;

            // Set Layout Bindings
            layout_bindings_.resize(1 + texture_count_);

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

            desc_layout_ = device.createDescriptorSetLayout(descriptor_layout_);

            // Set Pipeline Layout
            vk::PipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {};
            pPipelineLayoutCreateInfo.pNext                  = nullptr;
            pPipelineLayoutCreateInfo.pushConstantRangeCount = 0;
            pPipelineLayoutCreateInfo.pPushConstantRanges    = nullptr;
            pPipelineLayoutCreateInfo.setLayoutCount         = 1;
            pPipelineLayoutCreateInfo.pSetLayouts            = &desc_layout_;

            pipeline_layout_ = device.createPipelineLayout(pPipelineLayoutCreateInfo);

            //  Create Uniform Buffer
            auto app_data = ApplicationData::data;
            struct BufferData uniformBufferData = {};
            uniformBufferData.usage      = vk::BufferUsageFlagBits::eUniformBuffer;
            uniformBufferData.properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
            uniformBufferData.size       = sizeof(glm::mat4) * 3;

            uniform_buffer_ = new UniformBuffer(uniformBufferData);
            uniform_buffer_->initModelView(app_data->view_width, app_data->view_height);
        }

        Texture DescriptorSet::getTextelBuffer(const std::string& texture_path, vk::Queue queue)
        {
            struct ImageProps img_props = {};
            img_props.format     = vk::Format::eR8G8B8A8Unorm;
            img_props.aspectMask = vk::ImageAspectFlagBits::eColor;

            vk::Image texture_image = nullptr;
            if(!texture_path.empty()) {
                texture_image = Textures::createTextureImage(texture_path, queue);

                if(nullptr != texture_image) {

                    vk::SamplerCreateInfo sampler_ = {};
                    sampler_.maxAnisotropy 		= 1.0f;
                    sampler_.magFilter 			= vk::Filter::eLinear;
                    sampler_.minFilter 			= vk::Filter::eLinear;
                    sampler_.mipmapMode 		= vk::SamplerMipmapMode::eLinear;
                    sampler_.addressModeU 		= vk::SamplerAddressMode::eRepeat;
                    sampler_.addressModeV 		= vk::SamplerAddressMode::eRepeat;
                    sampler_.addressModeW 		= vk::SamplerAddressMode::eRepeat;
                    sampler_.mipLodBias 		= 0.0f;
                    sampler_.compareOp 			= vk::CompareOp::eNever;
                    sampler_.minLod 			= 0.0f;
                    sampler_.maxLod 			= 0.0f;
                    sampler_.maxAnisotropy 		= 1.0;
                    sampler_.anisotropyEnable 	= VK_FALSE;
                    sampler_.borderColor 		= vk::BorderColor::eFloatOpaqueWhite;

                    return Texture{
                        .buffer  = new Memory::BufferImage(img_props, texture_image),
                        .sampler = ApplicationData::data->device.createSampler(sampler_)
                    };
                }
            }

            assert(false);
        }

        void DescriptorSet::updateDescriptorSet(Texture texture, vk::DescriptorSet desc_set)
        {
            if(uniform_buffer_ == nullptr)
                return;

            std::vector<vk::WriteDescriptorSet, mem::StdAllocator<vk::WriteDescriptorSet>> writes = {};

            vk::DescriptorBufferInfo buffer_info;
            buffer_info.range  = uniform_buffer_->size;
            buffer_info.offset = 0;
            buffer_info.buffer = uniform_buffer_->buf;

            vk::WriteDescriptorSet write = {};
            write.pNext 			= nullptr;
            write.dstSet 			= desc_set;
            write.descriptorCount 	= 1;
            write.descriptorType 	= vk::DescriptorType::eUniformBuffer;
            write.pBufferInfo 		= &buffer_info;
            write.dstArrayElement 	= 0;
            write.dstBinding 		= 0;
            writes.push_back(write);

            if (texture_count_ > 0) {
                vk::DescriptorImageInfo texture_info = {};
                texture_info.imageLayout    = vk::ImageLayout::eShaderReadOnlyOptimal;
                texture_info.imageView 	    = texture.buffer->view;
                texture_info.sampler 	    = texture.sampler;

                write.dstSet 			    = desc_set;
                write.descriptorCount 	    = 1;
                write.descriptorType 	    = vk::DescriptorType::eCombinedImageSampler;
                write.pImageInfo 		    = &texture_info;
                write.dstArrayElement 	    = 0;
                write.dstBinding 		    = 1;
                writes.push_back(write);
            }

            ApplicationData::data->device.updateDescriptorSets(writes, {});
        }

        vk::DescriptorPool DescriptorSet::createDescriptorPool()
        {
            vk::DescriptorPool desc_pool;

            std::vector<vk::DescriptorPoolSize> poolSizes = {};

            poolSizes.resize(1 + texture_count_);
            poolSizes[0].type 			 = vk::DescriptorType::eUniformBuffer;
            poolSizes[0].descriptorCount = 1;

            if(texture_count_ > 0){
                poolSizes[1].type 			 = vk::DescriptorType::eCombinedImageSampler;
                poolSizes[1].descriptorCount = 1;
            }

            vk::DescriptorPoolCreateInfo descriptor_pool_info = {};
            descriptor_pool_info.pNext 			= nullptr;
            descriptor_pool_info.maxSets 		= 1;
            descriptor_pool_info.poolSizeCount 	= static_cast<uint32_t>(poolSizes.size());
            descriptor_pool_info.pPoolSizes 	= poolSizes.data();

            DEBUG_CALL(desc_pool = ApplicationData::data->device.createDescriptorPool(descriptor_pool_info));

            return desc_pool;
        }

        vk::DescriptorSet DescriptorSet::createDescriptorSet(vk::DescriptorPool desc_pool)
        {
            std::vector<vk::DescriptorSet> desc_sets_;

            vk::DescriptorSetAllocateInfo alloc_info_;
            alloc_info_.pNext 				= nullptr;
            alloc_info_.descriptorPool 		= desc_pool;
            alloc_info_.descriptorSetCount 	= 1;
            alloc_info_.pSetLayouts 		= &desc_layout_;

            DEBUG_CALL(desc_sets_ = ApplicationData::data->device.allocateDescriptorSets(alloc_info_));

            return desc_sets_[0];
        }

        vk::PipelineLayout DescriptorSet::getPipelineLayout() const
        {
            return pipeline_layout_;
        }

        UniformBuffer* DescriptorSet::getUniformBuffer() const
        {
            return uniform_buffer_;
        }
    }
}
