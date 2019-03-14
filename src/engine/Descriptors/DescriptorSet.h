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

        enum Type {
            GRAPHIC,
            COMPUTE
        };

        class DescriptorSet
        {

        private:

            std::vector<VkDescriptorSetLayoutBinding,
                mem::StdAllocator<
                    VkDescriptorSetLayoutBinding>>      _layout_bindings    = {};
            std::vector<VkDescriptorSetLayout,
                mem::StdAllocator<
                    VkDescriptorSetLayout>> 		    _desc_layout        = {};
            VkPipelineLayout 						    _pipeline_layout    = nullptr;
            Descriptors::UniformBuffer*                 _uniform_buffer     = nullptr;

            Type                                        _type               = Type::GRAPHIC;

        public:

            DescriptorSet(Type type) :  _type(type) {}

            ~DescriptorSet()
            {
                auto app_data = ApplicationData::data;

                vkDestroyPipelineLayout(app_data->device, _pipeline_layout, nullptr);
                for (auto &desc_layout : _desc_layout) {
                    vkDestroyDescriptorSetLayout(app_data->device, desc_layout, nullptr);
                }
                delete _uniform_buffer;
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
                if(_type == Type::GRAPHIC) {
                    _layout_bindings.resize(2);

                    _layout_bindings[0].binding 					 = 0;
                    _layout_bindings[0].descriptorType 				 = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    _layout_bindings[0].descriptorCount 			 = 1;
                    _layout_bindings[0].stageFlags 					 = VK_SHADER_STAGE_VERTEX_BIT;
                    _layout_bindings[0].pImmutableSamplers			 = nullptr;

                    _layout_bindings[1].binding 					 = 1;
                    _layout_bindings[1].descriptorType 				 = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    _layout_bindings[1].descriptorCount 			 = 1;
                    _layout_bindings[1].stageFlags 					 = VK_SHADER_STAGE_FRAGMENT_BIT;
                    _layout_bindings[1].pImmutableSamplers 			 = nullptr;
                }
                else if(_type == Type::COMPUTE) {
                    _layout_bindings.resize(1);

                    _layout_bindings[0].binding 					 = 1;
                    _layout_bindings[0].descriptorType 				 = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
                    _layout_bindings[0].descriptorCount 			 = 1;
                    _layout_bindings[0].stageFlags 					 = VK_SHADER_STAGE_COMPUTE_BIT;
                    _layout_bindings[0].pImmutableSamplers 			 = nullptr;
                } else {
                    Debug::logError("Descriptor Type " + std::to_string(_type) + "not supported!");
                    assert(false);
                }

                // Set Descriptor Layouts
                VkResult res;

                _desc_layout.resize(1);

                VkDescriptorSetLayoutCreateInfo  _descriptor_layout = {};
                _descriptor_layout.sType 						 = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                _descriptor_layout.pNext 						 = nullptr;
                _descriptor_layout.bindingCount 				 = static_cast<uint32_t>(_layout_bindings.size());
                _descriptor_layout.pBindings 					 = _layout_bindings.data();

                res = vkCreateDescriptorSetLayout(ApplicationData::data->device, &_descriptor_layout, nullptr, _desc_layout.data());
                assert(res == VK_SUCCESS);

                // Set Pipeline Layout
                VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {};
                pPipelineLayoutCreateInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
                pPipelineLayoutCreateInfo.pNext                  = nullptr;
                pPipelineLayoutCreateInfo.pushConstantRangeCount = 0;
                pPipelineLayoutCreateInfo.pPushConstantRanges    = nullptr;
                pPipelineLayoutCreateInfo.setLayoutCount         = 1;
                pPipelineLayoutCreateInfo.pSetLayouts            = _desc_layout.data();

                res = vkCreatePipelineLayout(ApplicationData::data->device, &pPipelineLayoutCreateInfo, nullptr, &_pipeline_layout);
                assert(res == VK_SUCCESS);

                //  Create Uniform Buffer
                auto app_data = ApplicationData::data;
                struct BufferData uniformBufferData = {};
                uniformBufferData.usage      = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
                uniformBufferData.properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
                uniformBufferData.size       = sizeof(glm::mat4)*3;

                _uniform_buffer = new UniformBuffer(uniformBufferData);
                _uniform_buffer->initModelView(app_data->view_width, app_data->view_height);
            }

            Texture getTextelBuffer(const std::string& texture_path, VkQueue queue)
            {
                auto app_data = ApplicationData::data;

                struct ImageProps img_props = {};
                img_props.format = VK_FORMAT_R8G8B8A8_UNORM;
                img_props.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

                if(_type == Type::GRAPHIC) {
                    VkImage texture_image = nullptr;
                    if(!texture_path.empty()) {
                        texture_image = Textures::createTextureImage(texture_path, queue);

                        if(texture_image != nullptr) {
                            return Texture{
                                .buffer  = new Memory::BufferImage(img_props, &texture_image),
                                .sampler = createSampler()
                            };
                        }
                    }
                }

                if (_type == Type::COMPUTE) {
                    return Texture{
                        .buffer = new Memory::BufferImage(img_props),
                    };
                }

                assert(false);
            }

            void updateDescriptorSet(Texture texture, VkDescriptorSet desc_set)
            {
                std::vector<VkWriteDescriptorSet, mem::StdAllocator<VkWriteDescriptorSet>> writes = {};

                VkWriteDescriptorSet write = {};
                write.sType 								  = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                write.pNext 								  = nullptr;
                write.dstSet 								  = desc_set;
                write.descriptorCount 						  = 1;
                write.descriptorType 						  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                write.pBufferInfo 							  = &_uniform_buffer->buffer_info;
                write.dstArrayElement 						  = 0;
                write.dstBinding 							  = 0;
                writes.push_back(write);

                VkDescriptorImageInfo texture_info = {};
                texture_info.imageLayout 					  = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                texture_info.imageView 						  = texture.buffer->view;
                texture_info.sampler 						  = texture.sampler;

                write = {};
                write.sType 								  = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                write.pNext 								  = nullptr;
                write.dstSet 								  = desc_set;
                write.descriptorCount 						  = 1;
                write.descriptorType 						  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                write.pImageInfo 							  = &texture_info;
                write.dstArrayElement 						  = 0;
                write.dstBinding 							  = 1;
                writes.push_back(write);

                vkUpdateDescriptorSets(ApplicationData::data->device, static_cast<u_int32_t>(writes.size()), writes.data(), 0, nullptr);
            }

            VkDescriptorPool createDescriptorPool()
            {
                VkDescriptorPool desc_pool;

                if(_type == Type::GRAPHIC) {
                    VkDescriptorPoolSize poolSizes[2];
                    poolSizes[0].type 								 = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    poolSizes[0].descriptorCount 					 = 1;

                    poolSizes[1].type 								 = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    poolSizes[1].descriptorCount 					 = 1;

                    VkDescriptorPoolCreateInfo descriptor_pool = {};
                    descriptor_pool.sType 							 = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
                    descriptor_pool.pNext 							 = nullptr;
                    descriptor_pool.maxSets 						 = 1;
                    descriptor_pool.poolSizeCount 					 = 2;
                    descriptor_pool.pPoolSizes 						 = poolSizes;

                    VkResult res = vkCreateDescriptorPool(ApplicationData::data->device, &descriptor_pool, nullptr, &desc_pool);
                    assert(res == VK_SUCCESS);

                    return desc_pool;
                }

                if(_type == Type::COMPUTE) {
                    VkDescriptorPoolSize type_count[3];
                    type_count[0].type 								 = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    type_count[0].descriptorCount 					 = 1;

                    type_count[1].type 								 = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    type_count[1].descriptorCount 					 = 1;

                    type_count[2].type 								 = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    type_count[2].descriptorCount 					 = 1;

                    VkDescriptorPoolCreateInfo descriptor_pool = {};
                    descriptor_pool.sType 							 = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
                    descriptor_pool.pNext 							 = nullptr;
                    descriptor_pool.maxSets 						 = 1;
                    descriptor_pool.poolSizeCount 					 = 3;
                    descriptor_pool.pPoolSizes 						 = type_count;

                    assert(vkCreateDescriptorPool(ApplicationData::data->device, &descriptor_pool, nullptr, &desc_pool) == VK_SUCCESS);

                    return desc_pool;
                }

                assert(false);
            }

            VkDescriptorSet createDescriptorSet(VkDescriptorPool desc_pool)
            {
                VkDescriptorSet _desc_set = nullptr;

                VkDescriptorSetAllocateInfo _alloc_info[1];
                _alloc_info[0].sType 							  = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
                _alloc_info[0].pNext 							  = nullptr;
                _alloc_info[0].descriptorPool 					  = desc_pool;
                _alloc_info[0].descriptorSetCount 				  = 1;
                _alloc_info[0].pSetLayouts 						  = _desc_layout.data();

                assert(vkAllocateDescriptorSets(ApplicationData::data->device, _alloc_info, &_desc_set) == VK_SUCCESS);

                return _desc_set;
            }

            VkPipelineLayout getPipelineLayout() const
            {
                return _pipeline_layout;
            }

            UniformBuffer* getUniformBuffer() const
            {
                return _uniform_buffer;
            }

        private:

            VkSampler createSampler()
            {
                VkSamplerCreateInfo sampler = {};
                sampler.sType 									  = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
                sampler.maxAnisotropy 							  = 1.0f;
                sampler.magFilter 								  = VK_FILTER_LINEAR;
                sampler.minFilter 								  = VK_FILTER_LINEAR;
                sampler.mipmapMode 								  = VK_SAMPLER_MIPMAP_MODE_LINEAR;
                sampler.addressModeU 							  = VK_SAMPLER_ADDRESS_MODE_REPEAT;
                sampler.addressModeV 							  = VK_SAMPLER_ADDRESS_MODE_REPEAT;
                sampler.addressModeW 							  = VK_SAMPLER_ADDRESS_MODE_REPEAT;
                sampler.mipLodBias 								  = 0.0f;
                sampler.compareOp 								  = VK_COMPARE_OP_NEVER;
                sampler.minLod 									  = 0.0f;
                sampler.maxLod 									  = 0.0f;
                sampler.maxAnisotropy 							  = 1.0;
                sampler.anisotropyEnable 						  = VK_FALSE;
                sampler.borderColor 							  = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

                VkSampler sampler_obj = {};

                assert(vkCreateSampler(ApplicationData::data->device, &sampler, nullptr, &sampler_obj) == VK_SUCCESS);

                return sampler_obj;
            }


        };
    }
}
#endif //OBSIDIAN2D_DESCRIPTORSET_H
