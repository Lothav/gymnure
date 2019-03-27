//
// Created by luizorv on 9/7/17.
//

#ifndef OBSIDIAN2D_GRAPHICPIPELINE_H
#define OBSIDIAN2D_GRAPHICPIPELINE_H

#include <ApplicationData.hpp>
#include "GraphicsPipeline.h"

namespace Engine
{
    namespace GraphicsPipeline
    {
        GraphicsPipeline::GraphicsPipeline(const std::vector<Shader>& shaders)
        {
            shader_stages_.resize(shaders.size());

            // Set shader stages
            for (int i = 0; i < shaders.size(); ++i) {
                shader_stages_[i].stage  = shaders[i].type;
                shader_stages_[i].module = Util::Util::loadSPIRVShader(shaders[i].path);
                shader_stages_[i].pName  = "main";
                assert(shader_stages_[i].module);
            }
        }

        GraphicsPipeline::~GraphicsPipeline()
        {
            vk::Device device = ApplicationData::data->device;

            device.destroyPipeline(pipeline_);
            device.destroyPipelineCache(pipeline_cache_);
        }

        vk::Pipeline GraphicsPipeline::getPipeline() const
        {
            return pipeline_;
        }

        void GraphicsPipeline::addViAttributes(const std::vector<vk::VertexInputAttributeDescription>& vi_attrs)
        {
            for (auto& vi_attr: vi_attrs)
                vi_attributes_.push_back(vi_attr);
        }

        void GraphicsPipeline::create(vk::PipelineLayout pipeline_layout, vk::RenderPass render_pass, vk::CullModeFlagBits cull_mode)
        {
            vk::Device device = ApplicationData::data->device;

            vk::VertexInputBindingDescription vi_binding = {};
            vi_binding.binding 					    = 0;
            vi_binding.inputRate 				    = vk::VertexInputRate::eVertex;
            vi_binding.stride 					    = sizeof(VertexData);

            vk::PipelineVertexInputStateCreateInfo vi = {};
            vi.pNext 								= nullptr;
            vi.vertexBindingDescriptionCount 		= 1;
            vi.pVertexBindingDescriptions 			= &vi_binding;
            vi.vertexAttributeDescriptionCount 		= static_cast<uint32_t>(vi_attributes_.size());
            vi.pVertexAttributeDescriptions 		= vi_attributes_.data();

            vk::PipelineCacheCreateInfo pipelineCache = {};
            pipelineCache.pNext 					= nullptr;
            pipelineCache.initialDataSize 			= 0;
            pipelineCache.pInitialData 				= nullptr;
            pipeline_cache_ = device.createPipelineCache(pipelineCache);

            vk::PipelineInputAssemblyStateCreateInfo ia = {};
            ia.pNext 								= nullptr;
            ia.primitiveRestartEnable 				= VK_FALSE;
            ia.topology 							= vk::PrimitiveTopology::eTriangleList;

            vk::PipelineRasterizationStateCreateInfo rs = {};
            rs.pNext 								= nullptr;
            rs.polygonMode 							= vk::PolygonMode::eFill;
            rs.cullMode 							= cull_mode;
            rs.frontFace 							= vk::FrontFace::eClockwise;
            rs.depthClampEnable 					= VK_FALSE;
            rs.rasterizerDiscardEnable 				= VK_FALSE;
            rs.depthBiasEnable 						= VK_FALSE;
            rs.depthBiasConstantFactor 				= 0;
            rs.depthBiasClamp 						= 0;
            rs.depthBiasSlopeFactor 				= 0;
            rs.lineWidth 							= 1.0f;

            std::array<vk::PipelineColorBlendAttachmentState, 1> att_state = {};
            att_state[0].colorWriteMask 			= vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB;
            att_state[0].blendEnable 				= VK_TRUE;
            att_state[0].srcColorBlendFactor        = vk::BlendFactor::eSrcAlpha;
            att_state[0].dstColorBlendFactor        = vk::BlendFactor::eOneMinusSrcAlpha;
            att_state[0].colorBlendOp               = vk::BlendOp::eAdd;
            att_state[0].alphaBlendOp               = vk::BlendOp::eAdd;
            att_state[0].srcAlphaBlendFactor        = vk::BlendFactor::eZero;
            att_state[0].dstAlphaBlendFactor        = vk::BlendFactor::eZero;

            vk::PipelineColorBlendStateCreateInfo cb = {};
            cb.pNext 							    = nullptr;
            cb.attachmentCount 					    = static_cast<uint32_t>(att_state.size());
            cb.pAttachments 					    = att_state.data();
            cb.logicOpEnable 					    = VK_FALSE;
            cb.logicOp 							    = vk::LogicOp::eNoOp;
            cb.blendConstants[0] 				    = 1.0f;
            cb.blendConstants[1] 				    = 1.0f;
            cb.blendConstants[2] 				    = 1.0f;
            cb.blendConstants[3] 				    = 1.0f;

            std::vector<vk::DynamicState, mem::StdAllocator<vk::DynamicState>>
                dynamicStateEnables = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};

            vk::PipelineViewportStateCreateInfo vp = {};
            vp.pNext 								= nullptr;
            vp.viewportCount 						= 1;
            vp.scissorCount 						= 1;
            vp.pScissors 							= nullptr;
            vp.pViewports 							= nullptr;

            vk::PipelineDynamicStateCreateInfo dynamicState = {};
            dynamicState.pNext 						= nullptr;
            dynamicState.pDynamicStates 			= dynamicStateEnables.data();
            dynamicState.dynamicStateCount 			= static_cast<uint32_t>(dynamicStateEnables.size());

            vk::PipelineDepthStencilStateCreateInfo ds = {};
            ds.pNext 								= nullptr;

            ds.depthTestEnable 						= VK_FALSE;
            ds.depthWriteEnable 					= VK_TRUE;
            ds.depthCompareOp 						= vk::CompareOp::eLess;
            ds.depthBoundsTestEnable 				= VK_FALSE;
            ds.minDepthBounds 						= 0.0f; // Optional
            ds.maxDepthBounds 						= 1.0f; // Optional

            ds.stencilTestEnable 					= VK_FALSE;
            ds.back.failOp 							= vk::StencilOp::eKeep;
            ds.back.passOp 							= vk::StencilOp::eKeep;
            ds.back.compareOp 						= vk::CompareOp::eAlways;
            ds.back.compareMask 					= 0;
            ds.back.reference 						= 0;
            ds.back.depthFailOp 					= vk::StencilOp::eKeep;
            ds.back.writeMask 						= 0;
            ds.minDepthBounds 						= 0;
            ds.maxDepthBounds 						= 0;
            ds.front 								= ds.back;

            vk::PipelineMultisampleStateCreateInfo ms = {};
            ms.pNext 								= nullptr;
            ms.pSampleMask 							= nullptr;
            ms.rasterizationSamples 				= vk::SampleCountFlagBits::e1;
            ms.sampleShadingEnable 					= VK_FALSE;
            ms.alphaToCoverageEnable 				= VK_FALSE;
            ms.alphaToOneEnable 					= VK_FALSE;
            ms.minSampleShading 					= 0.0;

            vk::GraphicsPipelineCreateInfo pipeline_info = {};
            pipeline_info.pNext 					= nullptr;
            pipeline_info.layout 					= pipeline_layout;
            pipeline_info.basePipelineHandle 		= nullptr;
            pipeline_info.basePipelineIndex 		= 0;
            pipeline_info.pVertexInputState 		= &vi;
            pipeline_info.pInputAssemblyState 		= &ia;
            pipeline_info.pRasterizationState 		= &rs;
            pipeline_info.pColorBlendState 			= &cb;
            pipeline_info.pTessellationState 		= nullptr;
            pipeline_info.pMultisampleState 		= &ms;
            pipeline_info.pDynamicState 			= &dynamicState;
            pipeline_info.pViewportState 			= &vp;
            pipeline_info.pDepthStencilState 		= &ds;
            pipeline_info.pStages 					= this->shader_stages_.data();
            pipeline_info.stageCount 				= static_cast<uint32_t>(this->shader_stages_.size());
            pipeline_info.renderPass 				= render_pass;
            pipeline_info.subpass 					= 0;

            pipeline_ = device.createGraphicsPipeline(pipeline_cache_, pipeline_info, nullptr);

            for (auto &shader_stage : shader_stages_) {
                device.destroyShaderModule(shader_stage.module, nullptr);
            }
        }

    }
}


#endif //OBSIDIAN2D_GRAPHICPIPELINE_H

