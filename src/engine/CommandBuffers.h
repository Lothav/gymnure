//
// Created by luizorv on 9/2/17.
//

#ifndef OBSIDIAN2D_COMMANDBUFFERS_H
#define OBSIDIAN2D_COMMANDBUFFERS_H

#include <vector>
#include <cassert>
#include <GraphicPipeline/GraphicPipeline.h>
#include <Programs/Program.h>
#include "RenderPass/RenderPass.h"
#include "Descriptors/DescriptorSet.h"
#include "SyncPrimitives/SyncPrimitives.h"
#include "Vertex/VertexBuffer.h"
#include <ApplicationData.hpp>
#include <memory>

namespace Engine
{
    class CommandBuffers
    {

    private:

        VkCommandBuffer                     _command_buffer{};

    public:

        explicit CommandBuffers()
        {
            auto app_data = ApplicationData::data;

            VkCommandBufferAllocateInfo cmd = {};
            cmd.sType 				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            cmd.pNext 			 	= nullptr;
            cmd.commandPool 	 	= app_data.graphic_command_pool;
            cmd.level 			 	= VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            cmd.commandBufferCount  = 1;

            assert(vkAllocateCommandBuffers(app_data.device, &cmd, &_command_buffer) == VK_SUCCESS);
        }

        ~CommandBuffers()
        {
            auto app_data = ApplicationData::data;
            vkFreeCommandBuffers(app_data.device, app_data.graphic_command_pool, 1, &_command_buffer);
        }

        void* operator new(std::size_t size)
        {
            return mem::Provider::getMemory(size);
        }

        void operator delete(void* ptr)
        {
            // Do not free memory here!
        }

        void bindGraphicCommandBuffer (
                std::vector<Programs::Program*>  programs,
                RenderPass::RenderPass* 	     render_pass,
                uint32_t 		                 width,
                uint32_t 		                 height,
                SyncPrimitives::SyncPrimitives*  sync_primitives
        ) {
            VkResult res;
            const VkDeviceSize offsets[1] = {0};

            VkClearValue clear_values[2];
            clear_values[0].color.float32[0] 			= 0.f;
            clear_values[0].color.float32[1] 			= 0.f;
            clear_values[0].color.float32[2] 			= 0.f;
            clear_values[0].color.float32[3] 			= 1.f;
            clear_values[1].depthStencil.depth 			= 1.f;
            clear_values[1].depthStencil.stencil 		= 0;

            VkRenderPassBeginInfo rp_begin = {};
            rp_begin.sType 								= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            rp_begin.pNext 								= nullptr;
            rp_begin.renderPass 						= render_pass->getRenderPass();
            rp_begin.renderArea.offset.x 				= 0;
            rp_begin.renderArea.offset.y 				= 0;
            rp_begin.renderArea.extent.width 			= width;
            rp_begin.renderArea.extent.height 			= height;
            rp_begin.clearValueCount 					= 2;
            rp_begin.pClearValues 						= clear_values;

            VkCommandBufferBeginInfo cmd_buf_info = {};
            cmd_buf_info.sType 							= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            cmd_buf_info.pNext 							= nullptr;
            cmd_buf_info.flags 							= 0;
            cmd_buf_info.pInheritanceInfo 				= nullptr;

            auto util = std::make_unique<Util::Util>(width, height);
            res = vkBeginCommandBuffer(_command_buffer, &cmd_buf_info);
            assert(res == VK_SUCCESS);

            for(uint32_t i = 0; i < render_pass->getSwapChain()->getImageCount(); i++)
            {
                auto frame_buffers = render_pass->getFrameBuffers();
                if(frame_buffers.size() <= i) assert(false);

                rp_begin.framebuffer = frame_buffers[i];
                vkCmdBeginRenderPass(_command_buffer, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);

                for(auto& program_obj : programs) {

                    for(auto &data : program_obj->data) {

                        vkCmdBindPipeline(_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, program_obj->graphic_pipeline->getPipeline());
                        vkCmdBindDescriptorSets(_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                                program_obj->descriptor_set->getPipelineLayout(), 0,
                                                1, &data->descriptor_set, 0, nullptr);
                        vkCmdBindVertexBuffers(_command_buffer, 0, 1, &data->vertex_buffer->buf, offsets);

                        util->init_viewports(_command_buffer);
                        util->init_scissors(_command_buffer);

                        vkCmdDraw(_command_buffer, static_cast<uint32_t>(data->vertex_buffer->getVertexSize()), 1, 0, 0);
                    }
                }

                vkCmdEndRenderPass(_command_buffer);
            }

            res = vkEndCommandBuffer(_command_buffer);
            assert(res == VK_SUCCESS);
        }

        VkCommandBuffer* getCommandBufferPtr()
        {
            return &_command_buffer;
        }

    };
}

#endif //OBSIDIAN2D_COMMANDBUFFERS_H
