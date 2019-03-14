//
// Created by luizorv on 9/7/17.
//

#ifndef OBSIDIAN2D_GRAPHICPIPELINE_H
#define OBSIDIAN2D_GRAPHICPIPELINE_H

#include <array>
#include <Provider.hpp>
#include <Allocator.hpp>
#include "Util/Util.h"

namespace Engine
{
	namespace GraphicPipeline
	{
		struct Shader {
			std::string 			path;
			VkShaderStageFlagBits 	type;
		};

		class GraphicPipeline
		{

		private:

            VkPipelineCache 						            _pipeline_cache{};
			VkPipeline 								            _vk_pipeline{};
			std::vector<VkPipelineShaderStageCreateInfo,
					mem::StdAllocator<
					        VkPipelineShaderStageCreateInfo>> 	_shader_stages;
            VkDevice                                            _instance_device;
            VkVertexInputBindingDescription                     _vi_binding{};
            std::vector<VkVertexInputAttributeDescription>      _vi_attributes;

		public:

			GraphicPipeline(VkDevice device, const std::vector<Shader>& shaders)
            {
                _instance_device = device;
				_shader_stages.resize(shaders.size());

				// Set shader stages
				for (int i = 0; i < shaders.size(); ++i) {
					_shader_stages[i].sType 			= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
					_shader_stages[i].stage 			= shaders[i].type;
					_shader_stages[i].module 			= Util::Util::loadSPIRVShader(shaders[i].path, _instance_device);
					_shader_stages[i].pName 			= "main";
					assert(_shader_stages[0].module != VK_NULL_HANDLE);
				}
            }

            ~GraphicPipeline()
            {
                vkDestroyPipeline(_instance_device, _vk_pipeline, nullptr);
                vkDestroyPipelineCache(_instance_device, _pipeline_cache, nullptr);
            }

			void* operator new(std::size_t size)
			{
				return mem::Provider::getMemory(size);
			}

			void operator delete(void* ptr)
			{
				// Do not free memory here!
			}

			VkPipeline getPipeline() const
			{
				return _vk_pipeline;
			}

			void addViAttributes(const VkVertexInputAttributeDescription& vi_attr)
            {
                _vi_attributes.push_back(vi_attr);
			}

			void addViAttributes(const std::vector<VkVertexInputAttributeDescription>& vi_attrs)
			{
				for (auto& vi_attr: vi_attrs) _vi_attributes.push_back(vi_attr);
			}

			void setViBinding(VkVertexInputBindingDescription vi_binding)
            {
                _vi_binding = vi_binding;
			}

			void create(VkPipelineLayout pipeline_layout, VkRenderPass render_pass, VkCullModeFlagBits cull_mode)
			{
				VkPipelineVertexInputStateCreateInfo vi = {};
				memset(&vi, 0, sizeof(vi));
				vi.sType 								= VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
				vi.pNext 								= nullptr;
				vi.flags 								= 0;
				vi.vertexBindingDescriptionCount 		= 1;
				vi.pVertexBindingDescriptions 			= &_vi_binding;
				vi.vertexAttributeDescriptionCount 		= static_cast<uint32_t>(_vi_attributes.size());
				vi.pVertexAttributeDescriptions 		= _vi_attributes.data();

				VkPipelineCacheCreateInfo pipelineCache = {};
				pipelineCache.sType 					= VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
				pipelineCache.pNext 					= nullptr;
				pipelineCache.initialDataSize 			= 0;
				pipelineCache.pInitialData 				= nullptr;
				pipelineCache.flags 					= 0;
				VkResult res = vkCreatePipelineCache(_instance_device, &pipelineCache, nullptr, &_pipeline_cache);
				assert(res == VK_SUCCESS);

				VkPipelineInputAssemblyStateCreateInfo ia = {};
				ia.sType 								= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
				ia.pNext 								= nullptr;
				ia.flags 								= 0;
				ia.primitiveRestartEnable 				= VK_FALSE;
				ia.topology 							= VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

				VkPipelineRasterizationStateCreateInfo rs = {};
				rs.sType 								= VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
				rs.pNext 								= nullptr;
				rs.flags 								= 0;
				rs.polygonMode 							= VK_POLYGON_MODE_FILL;
				rs.cullMode 							= cull_mode;
				rs.frontFace 							= VK_FRONT_FACE_CLOCKWISE;
				rs.depthClampEnable 					= VK_FALSE;
				rs.rasterizerDiscardEnable 				= VK_FALSE;
				rs.depthBiasEnable 						= VK_FALSE;
				rs.depthBiasConstantFactor 				= 0;
				rs.depthBiasClamp 						= 0;
				rs.depthBiasSlopeFactor 				= 0;
				rs.lineWidth 							= 1.0f;

				std::array<VkPipelineColorBlendAttachmentState, 1> att_state = {};
				att_state[0].colorWriteMask 			= 0xf;
				att_state[0].blendEnable 				= VK_FALSE;
				att_state[0].srcColorBlendFactor        = VK_BLEND_FACTOR_ZERO;
				att_state[0].dstColorBlendFactor        = VK_BLEND_FACTOR_ZERO;
				att_state[0].colorBlendOp               = VK_BLEND_OP_ADD;
                att_state[0].alphaBlendOp               = VK_BLEND_OP_ADD;
                att_state[0].srcAlphaBlendFactor        = VK_BLEND_FACTOR_ZERO;
                att_state[0].dstAlphaBlendFactor        = VK_BLEND_FACTOR_ZERO;

                VkPipelineColorBlendStateCreateInfo cb = {};
                cb.sType 							    = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
                cb.flags 							    = 0;
                cb.pNext 							    = nullptr;
                cb.attachmentCount 					    = static_cast<uint32_t>(att_state.size());
				cb.pAttachments 					    = att_state.data();
				cb.logicOpEnable 					    = VK_FALSE;
				cb.logicOp 							    = VK_LOGIC_OP_NO_OP;
				cb.blendConstants[0] 				    = 1.0f;
				cb.blendConstants[1] 				    = 1.0f;
				cb.blendConstants[2] 				    = 1.0f;
				cb.blendConstants[3] 				    = 1.0f;

				std::vector<VkDynamicState, mem::StdAllocator<VkDynamicState>>
						dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

				VkPipelineViewportStateCreateInfo vp = {};
				vp.sType 								= VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
				vp.pNext 								= nullptr;
				vp.flags 								= 0;
				vp.viewportCount 						= 1;
                vp.scissorCount 						= 1;
				vp.pScissors 							= nullptr;
				vp.pViewports 							= nullptr;

				VkPipelineDynamicStateCreateInfo dynamicState = {};
				dynamicState.sType 						= VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
				dynamicState.pNext 						= nullptr;
				dynamicState.pDynamicStates 			= dynamicStateEnables.data();
				dynamicState.dynamicStateCount 			= static_cast<uint32_t>(dynamicStateEnables.size());

				VkPipelineDepthStencilStateCreateInfo ds = {};
				ds.sType 								= VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
				ds.pNext 								= nullptr;
				ds.flags 								= 0;

				ds.depthTestEnable 						= VK_FALSE;
				ds.depthWriteEnable 					= VK_TRUE;
				ds.depthCompareOp 						= VK_COMPARE_OP_LESS;
				ds.depthBoundsTestEnable 				= VK_FALSE;
				ds.minDepthBounds 						= 0.0f; // Optional
				ds.maxDepthBounds 						= 1.0f; // Optional

				ds.stencilTestEnable 					= VK_FALSE;
				ds.back.failOp 							= VK_STENCIL_OP_KEEP;
				ds.back.passOp 							= VK_STENCIL_OP_KEEP;
				ds.back.compareOp 						= VK_COMPARE_OP_ALWAYS;
				ds.back.compareMask 					= 0;
				ds.back.reference 						= 0;
				ds.back.depthFailOp 					= VK_STENCIL_OP_KEEP;
				ds.back.writeMask 						= 0;
				ds.minDepthBounds 						= 0;
				ds.maxDepthBounds 						= 0;
				ds.front 								= ds.back;

				VkPipelineMultisampleStateCreateInfo ms = {};
				ms.sType 								= VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
				ms.pNext 								= nullptr;
				ms.flags 								= 0;
				ms.pSampleMask 							= nullptr;
				ms.rasterizationSamples 				= VK_SAMPLE_COUNT_1_BIT;
				ms.sampleShadingEnable 					= VK_FALSE;
				ms.alphaToCoverageEnable 				= VK_FALSE;
				ms.alphaToOneEnable 					= VK_FALSE;
				ms.minSampleShading 					= 0.0;

				VkGraphicsPipelineCreateInfo pipeline = {};
				pipeline.sType 							= VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
				pipeline.pNext 							= nullptr;
				pipeline.layout 						= pipeline_layout;
				pipeline.basePipelineHandle 			= VK_NULL_HANDLE;
				pipeline.basePipelineIndex 				= 0;
				pipeline.flags 							= 0;
				pipeline.pVertexInputState 				= &vi;
				pipeline.pInputAssemblyState 			= &ia;
				pipeline.pRasterizationState 			= &rs;
				pipeline.pColorBlendState 				= &cb;
				pipeline.pTessellationState 			= nullptr;
				pipeline.pMultisampleState 				= &ms;
				pipeline.pDynamicState 					= &dynamicState;
				pipeline.pViewportState 				= &vp;
				pipeline.pDepthStencilState 			= &ds;
				pipeline.pStages 						= this->_shader_stages.data();
				pipeline.stageCount 					= static_cast<uint32_t>(this->_shader_stages.size());
				pipeline.renderPass 					= render_pass;
				pipeline.subpass 						= 0;

				res = vkCreateGraphicsPipelines(_instance_device, _pipeline_cache, 1, &pipeline, nullptr, &_vk_pipeline);
				assert(res == VK_SUCCESS);

				for (auto &shader_stage : _shader_stages) {
					vkDestroyShaderModule(_instance_device, shader_stage.module, nullptr);
				}
			}

		};
	}
}


#endif //OBSIDIAN2D_GRAPHICPIPELINE_H
