//
// Created by luizorv on 9/7/17.
//

#ifndef OBSIDIAN2D_GRAPHICPIPELINE_H
#define OBSIDIAN2D_GRAPHICPIPELINE_H

#include "Util/Util.h"

namespace Engine
{
	namespace GraphicPipeline
	{
		class GraphicPipeline: public Util::Util
		{

		private:

            VkPipelineCache 						            _pipeline_cache;
			VkPipeline 								            _vk_pipeline;
			std::vector<VkPipelineShaderStageCreateInfo> 		shaderStages;

            VkDevice                                            _instance_device;

		public:

			GraphicPipeline(VkDevice device)
            {
                _instance_device = device;
            }

            ~GraphicPipeline()
            {
                vkDestroyPipeline(_instance_device, _vk_pipeline, nullptr);
                vkDestroyPipelineCache(_instance_device, _pipeline_cache, nullptr);

                for(u_int32_t i = 0; i < shaderStages.size(); i++)
                {
                    vkDestroyShaderModule(_instance_device, shaderStages[i].module, nullptr);
                }

            }

			VkPipeline getPipeline()
			{
				return _vk_pipeline;
			}

			void create(VkPipelineLayout pipeline_layout, VkRenderPass render_pass)
			{

                shaderStages.resize(2);

				shaderStages[0].sType 		= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				shaderStages[0].stage 		= VK_SHADER_STAGE_VERTEX_BIT;
				shaderStages[0].module 		= loadSPIRVShader("../../shaders/vert.spv", _instance_device);
				shaderStages[0].pName 		= "main";
				assert(shaderStages[0].module != VK_NULL_HANDLE);

				shaderStages[1].sType 		= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				shaderStages[1].stage 		= VK_SHADER_STAGE_FRAGMENT_BIT;
				shaderStages[1].module 		= loadSPIRVShader("../../shaders/frag.spv", _instance_device);
				shaderStages[1].pName 		= "main";
				assert(shaderStages[1].module != VK_NULL_HANDLE);


				VkVertexInputBindingDescription 		vi_binding;


				vi_binding.binding 										= 0;
				vi_binding.inputRate 									= VK_VERTEX_INPUT_RATE_VERTEX;
				vi_binding.stride 										= sizeof(VertexData);

				std::vector<VkVertexInputAttributeDescription> vi_attribs;
				vi_attribs.resize(3);

				vi_attribs[0].binding 									= 0;
				vi_attribs[0].location 									= 0;
				vi_attribs[0].format 									= VK_FORMAT_R32G32B32_SFLOAT;
				vi_attribs[0].offset 									= offsetof(VertexData, pos);

				vi_attribs[1].binding 									= 0;
				vi_attribs[1].location 									= 1;
				vi_attribs[1].format 									= VK_FORMAT_R32G32_SFLOAT;
				vi_attribs[1].offset 									= offsetof(VertexData, uv);

				vi_attribs[2].binding 									= 0;
				vi_attribs[2].location 									= 2;
				vi_attribs[2].format 									= VK_FORMAT_R32G32B32_SFLOAT;
				vi_attribs[2].offset 									= offsetof(VertexData, normal);

				VkPipelineCacheCreateInfo pipelineCache;
				pipelineCache.sType 									= VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
				pipelineCache.pNext 									= NULL;
				pipelineCache.initialDataSize 							= 0;
				pipelineCache.pInitialData 								= NULL;
				pipelineCache.flags 									= 0;
				VkResult res = vkCreatePipelineCache(_instance_device, &pipelineCache, NULL, &_pipeline_cache);
				assert(res == VK_SUCCESS);

				VkPipelineVertexInputStateCreateInfo vi;
				memset(&vi, 0, sizeof(vi));
				vi.sType 												= VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
				vi.pNext 												= NULL;
				vi.flags 												= 0;
				vi.vertexBindingDescriptionCount 						= 1;
				vi.pVertexBindingDescriptions 							= &vi_binding;
				vi.vertexAttributeDescriptionCount 						= 3;
				vi.pVertexAttributeDescriptions 						= vi_attribs.data();

				VkPipelineInputAssemblyStateCreateInfo ia;
				ia.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
				ia.pNext = NULL;
				ia.flags = 0;
				ia.primitiveRestartEnable = VK_FALSE;
				ia.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

				VkPipelineRasterizationStateCreateInfo rs;
				rs.sType 												= VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
				rs.pNext 												= NULL;
				rs.flags 												= 0;
				rs.polygonMode 											= VK_POLYGON_MODE_FILL;
				rs.cullMode 											= VK_CULL_MODE_BACK_BIT;
				rs.frontFace 											= VK_FRONT_FACE_CLOCKWISE;
				rs.depthClampEnable 									= VK_FALSE;
				rs.rasterizerDiscardEnable 								= VK_FALSE;
				rs.depthBiasEnable 										= VK_FALSE;
				rs.depthBiasConstantFactor 								= 0;
				rs.depthBiasClamp 										= 0;
				rs.depthBiasSlopeFactor 								= 0;
				rs.lineWidth 											= 1.0f;

				std::array<VkPipelineColorBlendAttachmentState, 1> att_state = {};
				att_state[0].colorWriteMask 							= 0xf;
				att_state[0].blendEnable 								= VK_FALSE;
				att_state[0].srcColorBlendFactor                        = VK_BLEND_FACTOR_ZERO;
				att_state[0].dstColorBlendFactor                        = VK_BLEND_FACTOR_ZERO;
				att_state[0].colorBlendOp                               = VK_BLEND_OP_ADD;
                att_state[0].alphaBlendOp                               = VK_BLEND_OP_ADD;
                att_state[0].srcAlphaBlendFactor                        = VK_BLEND_FACTOR_ZERO;
                att_state[0].dstAlphaBlendFactor                        = VK_BLEND_FACTOR_ZERO;

                VkPipelineColorBlendStateCreateInfo cb;
                cb.sType 												= VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
                cb.flags 												= 0;
                cb.pNext 												= NULL;
                cb.attachmentCount 										= static_cast<uint32_t>(att_state.size());
				cb.pAttachments 										= att_state.data();
				cb.logicOpEnable 										= VK_FALSE;
				cb.logicOp 												= VK_LOGIC_OP_NO_OP;
				cb.blendConstants[0] 									= 1.0f;
				cb.blendConstants[1] 									= 1.0f;
				cb.blendConstants[2] 									= 1.0f;
				cb.blendConstants[3] 									= 1.0f;

				std::vector<VkDynamicState> dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

				VkPipelineViewportStateCreateInfo vp = {};
				vp.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
				vp.pNext = NULL;
				vp.flags = 0;
				vp.viewportCount 										= 1;
                vp.scissorCount 										= 1;
				vp.pScissors 											= NULL;
				vp.pViewports 											= NULL;

				VkPipelineDynamicStateCreateInfo dynamicState = {};
				dynamicState.sType 										= VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
				dynamicState.pNext 										= nullptr;
				dynamicState.pDynamicStates 							= dynamicStateEnables.data();
				dynamicState.dynamicStateCount 							= static_cast<uint32_t>(dynamicStateEnables.size());

				VkPipelineDepthStencilStateCreateInfo ds;
				ds.sType 												= VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
				ds.pNext 												= NULL;
				ds.flags 												= 0;
				ds.depthTestEnable 										= (VkBool32) true;
				ds.depthWriteEnable 									= (VkBool32) true;
				ds.depthCompareOp 										= VK_COMPARE_OP_LESS_OR_EQUAL;
				ds.depthBoundsTestEnable 								= VK_FALSE;
				ds.stencilTestEnable 									= VK_FALSE;
				ds.back.failOp 											= VK_STENCIL_OP_KEEP;
				ds.back.passOp 											= VK_STENCIL_OP_KEEP;
				ds.back.compareOp 										= VK_COMPARE_OP_ALWAYS;
				ds.back.compareMask 									= 0;
				ds.back.reference 										= 0;
				ds.back.depthFailOp 									= VK_STENCIL_OP_KEEP;
				ds.back.writeMask 										= 0;
				ds.minDepthBounds 										= 0;
				ds.maxDepthBounds 										= 0;
				ds.stencilTestEnable 									= VK_FALSE;
				ds.front 												= ds.back;

				VkPipelineMultisampleStateCreateInfo ms;
				ms.sType 												= VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
				ms.pNext 												= NULL;
				ms.flags 												= 0;
				ms.pSampleMask 											= NULL;
				ms.rasterizationSamples 								= VK_SAMPLE_COUNT_1_BIT;
				ms.sampleShadingEnable 									= VK_FALSE;
				ms.alphaToCoverageEnable 								= VK_FALSE;
				ms.alphaToOneEnable 									= VK_FALSE;
				ms.minSampleShading 									= 0.0;

				VkGraphicsPipelineCreateInfo pipeline;
				pipeline.sType 											= VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
				pipeline.pNext 											= nullptr;
				pipeline.layout 										= pipeline_layout;
				pipeline.basePipelineHandle 							= VK_NULL_HANDLE;
				pipeline.basePipelineIndex 								= 0;
				pipeline.flags 											= 0;
				pipeline.pVertexInputState 								= &vi;
				pipeline.pInputAssemblyState 							= &ia;
				pipeline.pRasterizationState 							= &rs;
				pipeline.pColorBlendState 								= &cb;
				pipeline.pTessellationState 							= NULL;
				pipeline.pMultisampleState 								= &ms;
				pipeline.pDynamicState 									= &dynamicState;
				pipeline.pViewportState 								= &vp;
				pipeline.pDepthStencilState 							= &ds;
				pipeline.pStages 										= shaderStages.data();
				pipeline.stageCount 									= static_cast<uint32_t>(shaderStages.size());
				pipeline.renderPass 									= render_pass;
				pipeline.subpass 										= 0;

				res = vkCreateGraphicsPipelines(_instance_device, _pipeline_cache, 1, &pipeline, NULL, &_vk_pipeline);
				assert(res == VK_SUCCESS);
			}
		};
	}
}


#endif //OBSIDIAN2D_GRAPHICPIPELINE_H
