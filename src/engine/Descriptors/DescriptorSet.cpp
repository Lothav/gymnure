#include "DescriptorSet.h"

namespace Engine
{
    namespace Descriptors
    {
        DescriptorSet::~DescriptorSet()
        {
            auto device = ApplicationData::data->device;

            device.destroyPipelineLayout(pipeline_layout_);
            for (auto &desc_layout : desc_layout_)
                device.destroyDescriptorSetLayout(desc_layout);

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
                layout_bindings_[1].binding 					 = 1;
                layout_bindings_[1].descriptorType 				 = vk::DescriptorType::eCombinedImageSampler;
                layout_bindings_[1].descriptorCount 			 = 1;
                layout_bindings_[1].stageFlags 					 = vk::ShaderStageFlagBits::eFragment;
                layout_bindings_[1].pImmutableSamplers 			 = nullptr;
            }

            // Set Descriptor Layouts
            vk::Result res;

            desc_layout_.resize(1);

            vk::DescriptorSetLayoutCreateInfo descriptor_layout_ = {};
            descriptor_layout_.pNext 						 = nullptr;
            descriptor_layout_.bindingCount 				 = static_cast<uint32_t>(layout_bindings_.size());
            descriptor_layout_.pBindings 					 = layout_bindings_.data();

            res = device.createDescriptorSetLayout(&descriptor_layout_, nullptr, desc_layout_.data());
            assert(res == vk::Result::eSuccess);

            // Set Pipeline Layout
            vk::PipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {};
            pPipelineLayoutCreateInfo.pNext                  = nullptr;
            pPipelineLayoutCreateInfo.pushConstantRangeCount = 0;
            pPipelineLayoutCreateInfo.pPushConstantRanges    = nullptr;
            pPipelineLayoutCreateInfo.setLayoutCount         = 1;
            pPipelineLayoutCreateInfo.pSetLayouts            = desc_layout_.data();

            res = device.createPipelineLayout(&pPipelineLayoutCreateInfo, nullptr, &pipeline_layout_);
            assert(res == vk::Result::eSuccess);

            //  Create Uniform Buffer
            auto app_data = ApplicationData::data;
            struct BufferData uniformBufferData = {};
            uniformBufferData.usage      = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            uniformBufferData.properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            uniformBufferData.size       = sizeof(glm::mat4) * 3;

            uniform_buffer_ = new UniformBuffer(uniformBufferData);
            uniform_buffer_->initModelView(app_data->view_width, app_data->view_height);
        }

        Texture DescriptorSet::getTextelBuffer(const std::string& texture_path, vk::Queue queue)
        {
            struct ImageProps img_props = {};
            img_props.format = VK_FORMAT_R8G8B8A8_UNORM;
            img_props.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

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

                    vk::Sampler sampler_obj_ = ApplicationData::data->device.createSampler(sampler_, nullptr);

                    return Texture{
                        .buffer  = new Memory::BufferImage(img_props, texture_image),
                        .sampler = sampler_obj_
                    };
                }
            }

            assert(false);
        }

        void DescriptorSet::updateDescriptorSet(Texture texture, vk::DescriptorSet desc_set)
        {
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

            ApplicationData::data->device.updateDescriptorSets(static_cast<u_int32_t>(writes.size()), writes.data(), 0, nullptr);
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

            vk::Result res = ApplicationData::data->device.createDescriptorPool(&descriptor_pool_info, nullptr, &desc_pool);
            assert(res == vk::Result::eSuccess);

            return desc_pool;
        }

        vk::DescriptorSet DescriptorSet::createDescriptorSet(vk::DescriptorPool desc_pool)
        {
            vk::DescriptorSet desc_set_;

            vk::DescriptorSetAllocateInfo alloc_info_[1];
            alloc_info_[0].pNext 							  = nullptr;
            alloc_info_[0].descriptorPool 					  = desc_pool;
            alloc_info_[0].descriptorSetCount 				  = 1;
            alloc_info_[0].pSetLayouts 						  = desc_layout_.data();

            vk::Result res = ApplicationData::data->device.allocateDescriptorSets(alloc_info_, &desc_set_);
            assert(res == vk::Result::eSuccess);

            return desc_set_;
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
