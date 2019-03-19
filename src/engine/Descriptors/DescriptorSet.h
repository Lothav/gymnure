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
#include "Descriptors/Textures.h"
#include "Memory/BufferImage.h"

namespace Engine
{
    namespace Descriptors
    {
        struct Texture {
            Memory::BufferImage*    buffer  = nullptr;
            VkSampler               sampler = nullptr;
        };

        class DescriptorSet
        {

        private:

            std::vector<VkDescriptorSetLayoutBinding,
                mem::StdAllocator<
                    VkDescriptorSetLayoutBinding>>      layout_bindings_    = {};
            std::vector<VkDescriptorSetLayout,
                mem::StdAllocator<
                    VkDescriptorSetLayout>> 		    desc_layout_        = {};
            VkPipelineLayout 						    pipeline_layout_    = nullptr;
            Descriptors::UniformBuffer*                 uniform_buffer_     = nullptr;

            uint32_t                                    texture_count_      = 0;

        public:

            explicit DescriptorSet(uint32_t texture_count) : texture_count_(texture_count) {}

            ~DescriptorSet()
            {
                auto app_data = ApplicationData::data;

                vkDestroyPipelineLayout(app_data->device, pipeline_layout_, nullptr);
                for (auto &desc_layout : desc_layout_) {
                    vkDestroyDescriptorSetLayout(app_data->device, desc_layout, nullptr);
                }
                delete uniform_buffer_;
            }

            void* operator new(std::size_t size)
            {
                return mem::Provider::getMemory(size);
            }

            void operator delete(void* ptr)
            {
                // Do not free memory here!
            }

            void create()
            {
                // Set Layout Bindings
                layout_bindings_.resize(1 + texture_count_);

                layout_bindings_[0].binding 					 = 0;
                layout_bindings_[0].descriptorType 				 = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                layout_bindings_[0].descriptorCount 			 = 1;
                layout_bindings_[0].stageFlags 					 = VK_SHADER_STAGE_VERTEX_BIT;
                layout_bindings_[0].pImmutableSamplers			 = nullptr;

                if (texture_count_ > 0) {
                    layout_bindings_[1].binding 					 = 1;
                    layout_bindings_[1].descriptorType 				 = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    layout_bindings_[1].descriptorCount 			 = 1;
                    layout_bindings_[1].stageFlags 					 = VK_SHADER_STAGE_FRAGMENT_BIT;
                    layout_bindings_[1].pImmutableSamplers 			 = nullptr;
                }

                // Set Descriptor Layouts
                VkResult res;

                desc_layout_.resize(1);

                VkDescriptorSetLayoutCreateInfo descriptor_layout_ = {};
                descriptor_layout_.sType 						 = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                descriptor_layout_.pNext 						 = nullptr;
                descriptor_layout_.bindingCount 				 = static_cast<uint32_t>(layout_bindings_.size());
                descriptor_layout_.pBindings 					 = layout_bindings_.data();

                res = vkCreateDescriptorSetLayout(ApplicationData::data->device, &descriptor_layout_, nullptr, desc_layout_.data());
                assert(res == VK_SUCCESS);

                // Set Pipeline Layout
                VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {};
                pPipelineLayoutCreateInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
                pPipelineLayoutCreateInfo.pNext                  = nullptr;
                pPipelineLayoutCreateInfo.pushConstantRangeCount = 0;
                pPipelineLayoutCreateInfo.pPushConstantRanges    = nullptr;
                pPipelineLayoutCreateInfo.setLayoutCount         = 1;
                pPipelineLayoutCreateInfo.pSetLayouts            = desc_layout_.data();

                res = vkCreatePipelineLayout(ApplicationData::data->device, &pPipelineLayoutCreateInfo, nullptr, &pipeline_layout_);
                assert(res == VK_SUCCESS);

                //  Create Uniform Buffer
                auto app_data = ApplicationData::data;
                struct BufferData uniformBufferData = {};
                uniformBufferData.usage      = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
                uniformBufferData.properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
                uniformBufferData.size       = sizeof(glm::mat4)*3;

                uniform_buffer_ = new UniformBuffer(uniformBufferData);
                uniform_buffer_->initModelView(app_data->view_width, app_data->view_height);
            }

            Texture getTextelBuffer(const std::string& texture_path, VkQueue queue)
            {
                auto app_data = ApplicationData::data;

                struct ImageProps img_props = {};
                img_props.format = VK_FORMAT_R8G8B8A8_UNORM;
                img_props.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

                VkImage texture_image = nullptr;
                if(!texture_path.empty()) {
                    texture_image = Textures::createTextureImage(texture_path, queue);

                    if(texture_image != nullptr) {

                        VkSamplerCreateInfo sampler_ = {};
                        sampler_.sType 				= VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
                        sampler_.maxAnisotropy 		= 1.0f;
                        sampler_.magFilter 			= VK_FILTER_LINEAR;
                        sampler_.minFilter 			= VK_FILTER_LINEAR;
                        sampler_.mipmapMode 		= VK_SAMPLER_MIPMAP_MODE_LINEAR;
                        sampler_.addressModeU 		= VK_SAMPLER_ADDRESS_MODE_REPEAT;
                        sampler_.addressModeV 		= VK_SAMPLER_ADDRESS_MODE_REPEAT;
                        sampler_.addressModeW 		= VK_SAMPLER_ADDRESS_MODE_REPEAT;
                        sampler_.mipLodBias 		= 0.0f;
                        sampler_.compareOp 			= VK_COMPARE_OP_NEVER;
                        sampler_.minLod 			= 0.0f;
                        sampler_.maxLod 			= 0.0f;
                        sampler_.maxAnisotropy 		= 1.0;
                        sampler_.anisotropyEnable 	= VK_FALSE;
                        sampler_.borderColor 		= VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

                        VkSampler sampler_obj_ = {};
                        assert(vkCreateSampler(ApplicationData::data->device, &sampler_, nullptr, &sampler_obj_) == VK_SUCCESS);

                        return Texture{
                            .buffer  = new Memory::BufferImage(img_props, &texture_image),
                            .sampler = sampler_obj_
                        };
                    }
                }

                assert(false);
            }

            void updateDescriptorSet(Texture texture, VkDescriptorSet desc_set)
            {
                std::vector<VkWriteDescriptorSet, mem::StdAllocator<VkWriteDescriptorSet>> writes = {};

                VkDescriptorBufferInfo buffer_info;
                buffer_info.range  = uniform_buffer_->size;
                buffer_info.offset = 0;
                buffer_info.buffer = uniform_buffer_->buf;

                VkWriteDescriptorSet write = {};
                write.sType 			= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                write.pNext 			= nullptr;
                write.dstSet 			= desc_set;
                write.descriptorCount 	= 1;
                write.descriptorType 	= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                write.pBufferInfo 		= &buffer_info;
                write.dstArrayElement 	= 0;
                write.dstBinding 		= 0;
                writes.push_back(write);

                if (texture_count_ > 0) {
                    VkDescriptorImageInfo texture_info = {};
                    texture_info.imageLayout    = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    texture_info.imageView 	    = texture.buffer->view;
                    texture_info.sampler 	    = texture.sampler;

                    write = {};
                    write.sType 			    = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    write.pNext 			    = nullptr;
                    write.dstSet 			    = desc_set;
                    write.descriptorCount 	    = 1;
                    write.descriptorType 	    = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    write.pImageInfo 		    = &texture_info;
                    write.dstArrayElement 	    = 0;
                    write.dstBinding 		    = 1;
                    writes.push_back(write);
                }

                vkUpdateDescriptorSets(ApplicationData::data->device, static_cast<u_int32_t>(writes.size()), writes.data(), 0, nullptr);
            }

            VkDescriptorPool createDescriptorPool()
            {
                VkDescriptorPool desc_pool;

                std::vector<VkDescriptorPoolSize> poolSizes = {};

                poolSizes.resize(1 + texture_count_);
                poolSizes[0].type 			 = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                poolSizes[0].descriptorCount = 1;

                if(texture_count_ > 0){
                    poolSizes[1].type 			 = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    poolSizes[1].descriptorCount = 1;
                }

                VkDescriptorPoolCreateInfo descriptor_pool = {};
                descriptor_pool.sType 							 = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
                descriptor_pool.pNext 							 = nullptr;
                descriptor_pool.maxSets 						 = 1;
                descriptor_pool.poolSizeCount 					 = static_cast<uint32_t>(poolSizes.size());
                descriptor_pool.pPoolSizes 						 = poolSizes.data();

                VkResult res = vkCreateDescriptorPool(ApplicationData::data->device, &descriptor_pool, nullptr, &desc_pool);
                assert(res == VK_SUCCESS);

                return desc_pool;
            }

            VkDescriptorSet createDescriptorSet(VkDescriptorPool desc_pool)
            {
                VkDescriptorSet desc_set_ = nullptr;

                VkDescriptorSetAllocateInfo alloc_info_[1];
                alloc_info_[0].sType 							  = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
                alloc_info_[0].pNext 							  = nullptr;
                alloc_info_[0].descriptorPool 					  = desc_pool;
                alloc_info_[0].descriptorSetCount 				  = 1;
                alloc_info_[0].pSetLayouts 						  = desc_layout_.data();

                assert(vkAllocateDescriptorSets(ApplicationData::data->device, alloc_info_, &desc_set_) == VK_SUCCESS);

                return desc_set_;
            }

            VkPipelineLayout getPipelineLayout() const
            {
                return pipeline_layout_;
            }

            UniformBuffer* getUniformBuffer() const
            {
                return uniform_buffer_;
            }
        };
    }
}
#endif //OBSIDIAN2D_DESCRIPTORSET_H
