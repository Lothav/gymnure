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
            _shader_stages.resize(shaders.size());

            // Set shader stages
            for (int i = 0; i < shaders.size(); ++i) {
                _shader_stages[i].stage  = shaders[i].type;
                _shader_stages[i].module = Util::Util::loadSPIRVShader(shaders[i].path);
                _shader_stages[i].pName  = "main";
                assert(_shader_stages[i].module);
            }
        }

        GraphicsPipeline::~GraphicsPipeline()
        {
            vk::Device device = ApplicationData::data->device;

            device.destroyPipeline(_vk_pipeline);
            device.destroyPipelineCache(_pipeline_cache);
        }

        vk::Pipeline GraphicsPipeline::getPipeline() const
        {
            return _vk_pipeline;
        }

        void GraphicsPipeline::addViAttributes(const vk::VertexInputAttributeDescription& vi_attr)
        {
            _vi_attributes.push_back(vi_attr);
        }

        void GraphicsPipeline::addViAttributes(const std::vector<vk::VertexInputAttributeDescription>& vi_attrs)
        {
            for (auto& vi_attr: vi_attrs) _vi_attributes.push_back(vi_attr);
        }

        void GraphicsPipeline::setViBinding(vk::VertexInputBindingDescription vi_binding)
        {
            _vi_binding = vi_binding;
        }

        void GraphicsPipeline::create(vk::PipelineLayout pipeline_layout, vk::RenderPass render_pass, vk::CullModeFlagBits cull_mode)
        {
            vk::Device device = ApplicationData::data->device;

            vk::PipelineVertexInputStateCreateInfo vi = {};
            vi.pNext 								= nullptr;
            vi.vertexBindingDescriptionCount 		= 1;
            vi.pVertexBindingDescriptions 			= &_vi_binding;
            vi.vertexAttributeDescriptionCount 		= static_cast<uint32_t>(_vi_attributes.size());
            vi.pVertexAttributeDescriptions 		= _vi_attributes.data();

            vk::PipelineCacheCreateInfo pipelineCache = {};
            pipelineCache.pNext 					= nullptr;
            pipelineCache.initialDataSize 			= 0;
            pipelineCache.pInitialData 				= nullptr;
            _pipeline_cache = device.createPipelineCache(pipelineCache);

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
            att_state[0].blendEnable 				= VK_FALSE;
            att_state[0].srcColorBlendFactor        = vk::BlendFactor::eZero;
            att_state[0].dstColorBlendFactor        = vk::BlendFactor::eZero;
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
            pipeline_info.pStages 					= this->_shader_stages.data();
            pipeline_info.stageCount 				= static_cast<uint32_t>(this->_shader_stages.size());
            pipeline_info.renderPass 				= render_pass;
            pipeline_info.subpass 					= 0;

            _vk_pipeline = device.createGraphicsPipeline(_pipeline_cache, pipeline_info, nullptr);

            for (auto &shader_stage : _shader_stages) {
                device.destroyShaderModule(shader_stage.module, nullptr);
            }
        }

    }
}


#endif //OBSIDIAN2D_GRAPHICPIPELINE_H

