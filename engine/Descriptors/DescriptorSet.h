//
// Created by tracksale on 9/4/17.
//

#ifndef OBSIDIAN2D_DESCRIPTORSET_H
#define OBSIDIAN2D_DESCRIPTORSET_H

#include "Descriptors/UniformBuffer.h"
#include "Descriptors/Textures.h"
#include "Memory/BufferImage.h"

struct DescriptorSetParams {
    VkPhysicalDevice                    gpu;
    uint32_t                            width;
    uint32_t                            height;
    VkQueue                             graphic_queue;
    VkCommandPool                       command_pool;
    VkPhysicalDeviceMemoryProperties    memory_properties;
    const char*                         path;
};

namespace Engine
{
    namespace Descriptors
    {
        class DescriptorSet
        {

        private:

            std::vector<VkDescriptorSetLayoutBinding>   _layout_bindings = {};
            std::vector<VkDescriptorSetLayout> 		    _desc_layout = {};
            VkPipelineLayout 						    _pipeline_layout = nullptr;
            VkDescriptorPool                            _desc_pool = nullptr;
            VkDescriptorSet                             _desc_set = nullptr;

            Descriptors::UniformBuffer*                 _uniform_buffer = nullptr;

            Memory::BufferImage*                        _textel_buffer = nullptr;
            VkSampler                                   _texture_sampler = nullptr;

            VkDevice                                    _instance_device;

        public:

            DescriptorSet(VkDevice device)
            {
                _instance_device = device;
            }

			~DescriptorSet()
			{
				delete _textel_buffer;
				delete _uniform_buffer;
				vkDestroySampler(_instance_device, _texture_sampler, nullptr);
				vkDestroyDescriptorPool(_instance_device, _desc_pool, nullptr);
				for (u_int32_t i = 0; i < _desc_layout.size(); i++) {
					vkDestroyDescriptorSetLayout(_instance_device, _desc_layout[i], NULL);
				}
				vkDestroyPipelineLayout(_instance_device, _pipeline_layout, NULL);
			}

            void create(struct DescriptorSetParams ds_params)
            {
                setLayoutBindings();
                setDescriptorLayouts();
                setPipelineLayout();
                setDescriptorPool();
                setDescriptorSet();

                /*  create Uniform Buffer  */

                struct BufferData uniformBufferData = {};

                uniformBufferData.device            = _instance_device;
                uniformBufferData.usage             = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
                uniformBufferData.physicalDevice    = ds_params.gpu;
                uniformBufferData.properties        = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
                uniformBufferData.size              = sizeof(glm::mat4);

                _uniform_buffer = new UniformBuffer(uniformBufferData);
                _uniform_buffer->initModelView(ds_params.width, ds_params.height);

                VkImage texture_image = NULL;
                if(ds_params.path != nullptr) {
                    texture_image = Textures::createTextureImage(ds_params.gpu, _instance_device, ds_params.path,
                                                                         ds_params.command_pool, ds_params.graphic_queue, ds_params.memory_properties);
                }
                struct MemoryProps mem_props = {};
                mem_props.device = _instance_device;

                struct ImageProps img_props = {};
                img_props.format = VK_FORMAT_R8G8B8A8_UNORM;
                img_props.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

                /*  create Textel Buffer  */

                if(texture_image != NULL) {
                    _textel_buffer = new Memory::BufferImage(mem_props, img_props, &texture_image);
                    createSampler();
                }

                updateDescriptorSet();
            }

            VkPipelineLayout getPipelineLayout()
            {
                return _pipeline_layout;
            }

            VkDescriptorSet* getDescriptorSet()
            {
                return &_desc_set;
            }

            UniformBuffer* getUniformBuffer()
            {
                return _uniform_buffer;
            }

        private:

            void setLayoutBindings()
            {
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

            void setDescriptorLayouts()
            {
                VkResult res;
                VkDescriptorSetLayoutCreateInfo  _descriptor_layout = {};
                _descriptor_layout.sType 						 = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                _descriptor_layout.pNext 						 = nullptr;
                _descriptor_layout.bindingCount 				 = static_cast<uint32_t>(_layout_bindings.size());
                _descriptor_layout.pBindings 					 = _layout_bindings.data();

                _desc_layout.resize(1);
                res = vkCreateDescriptorSetLayout(_instance_device, &_descriptor_layout, nullptr, _desc_layout.data());
                assert(res == VK_SUCCESS);
            }

            void setPipelineLayout()
            {
                VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {};
                pPipelineLayoutCreateInfo.sType                         = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
                pPipelineLayoutCreateInfo.pNext                         = nullptr;
                pPipelineLayoutCreateInfo.pushConstantRangeCount        = 0;
                pPipelineLayoutCreateInfo.pPushConstantRanges           = nullptr;
                pPipelineLayoutCreateInfo.setLayoutCount                = 1;
                pPipelineLayoutCreateInfo.pSetLayouts                   = _desc_layout.data();

                VkResult res = vkCreatePipelineLayout(_instance_device, &pPipelineLayoutCreateInfo, nullptr, &_pipeline_layout);
                assert(res == VK_SUCCESS);
            }

            void setDescriptorPool()
            {
                VkDescriptorPoolSize type_count[2];
                type_count[0].type 										= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                type_count[0].descriptorCount 							= 1;

                type_count[1].type 									    = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                type_count[1].descriptorCount 						    = 1;

                VkDescriptorPoolCreateInfo descriptor_pool = {};
                descriptor_pool.sType 									= VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
                descriptor_pool.pNext 									= NULL;
                descriptor_pool.maxSets 								= 1;
                descriptor_pool.poolSizeCount 							= 2;
                descriptor_pool.pPoolSizes 								= type_count;

                VkResult res = vkCreateDescriptorPool(_instance_device, &descriptor_pool, NULL, &_desc_pool);
                assert(res == VK_SUCCESS);
            }

            void setDescriptorSet()
            {
                VkDescriptorSetAllocateInfo _alloc_info[1];
                _alloc_info[0].sType 									= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
                _alloc_info[0].pNext 									= NULL;
                _alloc_info[0].descriptorPool 							= _desc_pool;
                _alloc_info[0].descriptorSetCount 						= 1;
                _alloc_info[0].pSetLayouts 								= _desc_layout.data();

                VkResult res = vkAllocateDescriptorSets(_instance_device, _alloc_info, &_desc_set);
                assert(res == VK_SUCCESS);
            }

            void createSampler()
            {
                VkSamplerCreateInfo sampler = {};
                sampler.sType 											= VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
                sampler.maxAnisotropy 									= 1.0f;
                sampler.magFilter 										= VK_FILTER_LINEAR;
                sampler.minFilter 										= VK_FILTER_LINEAR;
                sampler.mipmapMode 										= VK_SAMPLER_MIPMAP_MODE_LINEAR;
                sampler.addressModeU 									= VK_SAMPLER_ADDRESS_MODE_REPEAT;
                sampler.addressModeV 									= VK_SAMPLER_ADDRESS_MODE_REPEAT;
                sampler.addressModeW 									= VK_SAMPLER_ADDRESS_MODE_REPEAT;
                sampler.mipLodBias 										= 0.0f;
                sampler.compareOp 										= VK_COMPARE_OP_NEVER;
                sampler.minLod 											= 0.0f;
                sampler.maxLod 											= 0.0f;
                sampler.maxAnisotropy 									= 1.0;
                sampler.anisotropyEnable 								= VK_FALSE;
                sampler.borderColor 									= VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

                assert(vkCreateSampler(_instance_device, &sampler, nullptr, &_texture_sampler) == VK_SUCCESS);
            }

            void updateDescriptorSet()
            {

                std::vector<VkWriteDescriptorSet> writes = {};

                VkWriteDescriptorSet write = {};
                write.sType 										= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                write.pNext 										= nullptr;
                write.dstSet 										= _desc_set;
                write.descriptorCount 								= 1;
                write.descriptorType 								= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                write.pBufferInfo 									= &_uniform_buffer->buffer_info;
                write.dstArrayElement 								= 0;
                write.dstBinding 									= 0;
                writes.push_back(write);

                if(_textel_buffer != nullptr) {
                    VkDescriptorImageInfo texture_info;
                    texture_info.imageLayout 								= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    texture_info.imageView 									= _textel_buffer->view;
                    texture_info.sampler 									= _texture_sampler;

                    write = {};
                    write.sType 										= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    write.dstSet 										= _desc_set;
                    write.dstBinding 									= 1;
                    write.descriptorCount 								= 1;
                    write.descriptorType 								= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    write.pImageInfo 									= &texture_info;
                    write.dstArrayElement 								= 0;
                    writes.push_back(write);
                }

                vkUpdateDescriptorSets(_instance_device, static_cast<u_int32_t>(writes.size()), writes.data(), 0, nullptr);
            }
        };
    }
}
#endif //OBSIDIAN2D_DESCRIPTORSET_H
