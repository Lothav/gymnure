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
#include <memory>

namespace Engine
{
    class CommandBuffers
    {

    private:

        std::vector<VkCommandBuffer> command_buffers_ = {};

    public:

        explicit CommandBuffers(uint swapchain_images_count)
        {
            auto app_data = ApplicationData::data;

            VkCommandBufferAllocateInfo cmd = {};
            cmd.sType 				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            cmd.pNext 			 	= nullptr;
            cmd.commandPool 	 	= app_data->graphic_command_pool;
            cmd.level 			 	= VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            cmd.commandBufferCount  = swapchain_images_count;

            command_buffers_.resize(swapchain_images_count);
            auto res = vkAllocateCommandBuffers(app_data->device, &cmd, command_buffers_.data());
            assert(res == VK_SUCCESS);
        }

        ~CommandBuffers()
        {
            auto app_data = ApplicationData::data;
            vkFreeCommandBuffers(app_data->device, app_data->graphic_command_pool,
                                 static_cast<uint32_t>(command_buffers_.size()), command_buffers_.data());
        }

        void* operator new(std::size_t size)
        {
            return mem::Provider::getMemory(size);
        }

        void operator delete(void* ptr)
        {
            // Do not free memory here!
        }

        void bindGraphicCommandBuffer(std::vector<Programs::Program*> programs, RenderPass::RenderPass* render_pass, uint32_t width, uint32_t height)
        {
            VkResult res;
            const VkDeviceSize offsets[1] = {0};

            auto frame_buffers = render_pass->getFrameBuffers();
            auto image_count = render_pass->getSwapChain()->getImageCount();

            if(frame_buffers.size() != image_count) assert(false);

            VkCommandBufferBeginInfo cmd_buf_info = {};
            cmd_buf_info.sType 							= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            cmd_buf_info.pNext 							= nullptr;
            cmd_buf_info.flags 							= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
            cmd_buf_info.pInheritanceInfo 				= nullptr;

            VkClearValue clear_values[2];
            clear_values[0].color = { { 0.2f, 0.2f, 0.2f, 1.0f } };
            clear_values[1].depthStencil = { 1.0f, 0 };

            VkRenderPassBeginInfo rp_begin = {};
            rp_begin.sType 								= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            rp_begin.pNext 								= nullptr;
            rp_begin.renderPass 						= render_pass->getRenderPass();
            rp_begin.renderArea.offset 				    = {0, 0};
            rp_begin.renderArea.extent.width 			= width;
            rp_begin.renderArea.extent.height 			= height;
            rp_begin.clearValueCount 					= 2;
            rp_begin.pClearValues 						= clear_values;

            for(uint32_t i = 0; i < image_count; i++)
            {
                res = vkBeginCommandBuffer(command_buffers_[i], &cmd_buf_info);
                assert(res == VK_SUCCESS);

                rp_begin.framebuffer = frame_buffers[i];
                vkCmdBeginRenderPass(command_buffers_[i], &rp_begin, VK_SUBPASS_CONTENTS_INLINE);

                for(auto& program_obj : programs) {

                    for(auto &data : program_obj->data) {

                        vkCmdBindPipeline(command_buffers_[i], VK_PIPELINE_BIND_POINT_GRAPHICS, program_obj->graphic_pipeline->getPipeline());

                        vkCmdBindDescriptorSets(command_buffers_[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                                                program_obj->descriptor_set->getPipelineLayout(), 0,
                                                1, &data->descriptor_set, 0, nullptr);

                        vkCmdBindVertexBuffers(command_buffers_[i], 0, 1, &data->vertex_buffer->buf, offsets);

                        Util::Util::initViewports(command_buffers_[i], width, height);
                        Util::Util::initScissors(command_buffers_[i], width, height);

                        vkCmdDraw(command_buffers_[i], data->vertex_buffer->getVertexSize(), 1, 0, 0);
                    }
                }

                vkCmdEndRenderPass(command_buffers_[i]);

                res = vkEndCommandBuffer(command_buffers_[i]);
                assert(res == VK_SUCCESS);
            }
        }

        std::vector<VkCommandBuffer> getCommandBuffers()
        {
            return command_buffers_;
        }

    };
}

#endif //OBSIDIAN2D_COMMANDBUFFERS_H
