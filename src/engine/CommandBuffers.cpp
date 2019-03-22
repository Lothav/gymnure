#include "CommandBuffers.h"

namespace Engine
{
    CommandBuffers::CommandBuffers(uint swapchain_images_count)
    {
        auto app_data = ApplicationData::data;

        vk::CommandBufferAllocateInfo cmd_buff_ai = {};
        cmd_buff_ai.pNext 			 	= nullptr;
        cmd_buff_ai.commandPool 	 	= app_data->graphic_command_pool;
        cmd_buff_ai.level 			 	= vk::CommandBufferLevel::ePrimary;
        cmd_buff_ai.commandBufferCount  = swapchain_images_count;

        command_buffers_ = app_data->device.allocateCommandBuffers(cmd_buff_ai);
    }

    CommandBuffers::~CommandBuffers()
    {
        auto app_data = ApplicationData::data;
        app_data->device.freeCommandBuffers(app_data->graphic_command_pool,
                                            static_cast<uint32_t>(command_buffers_.size()), command_buffers_.data());
    }

    void CommandBuffers::bindGraphicCommandBuffer(std::vector<Programs::Program*> programs, RenderPass::RenderPass* render_pass, uint32_t width, uint32_t height)
    {
        auto frame_buffers = render_pass->getFrameBuffers();
        auto image_count = render_pass->getSwapChain()->getImageCount();

        if(frame_buffers.size() != image_count) assert(false);

        vk::CommandBufferBeginInfo cmd_buf_info = {};
        cmd_buf_info.pNext 				= nullptr;
        cmd_buf_info.flags 				= vk::CommandBufferUsageFlagBits::eSimultaneousUse;
        cmd_buf_info.pInheritanceInfo 	= nullptr;

        vk::ClearValue clear_values[2];
        std::array<float,4> clear_values_arr = { 0.8f, 0.2f, 0.8f, 1.0f };
        clear_values[0].color = vk::ClearColorValue(clear_values_arr);
        clear_values[1].depthStencil = vk::ClearDepthStencilValue{1.0f, 0u};

        vk::RenderPassBeginInfo rp_begin = {};
        rp_begin.pNext 				= nullptr;
        rp_begin.renderPass 		= render_pass->getRenderPass();
        rp_begin.renderArea.offset 	= vk::Offset2D{0, 0};
        rp_begin.renderArea.extent  = vk::Extent2D{width, height};
        rp_begin.clearValueCount 	= 2;
        rp_begin.pClearValues 		= clear_values;

        for(uint32_t i = 0; i < image_count; i++)
        {
            command_buffers_[i].begin(cmd_buf_info);

            rp_begin.setFramebuffer(frame_buffers[i]);
            command_buffers_[i].beginRenderPass(rp_begin, vk::SubpassContents::eInline);

            for(auto& program_obj : programs) {

                for(auto &data : program_obj->data) {

                    Util::Util::initViewport(command_buffers_[i], width, height);
                    Util::Util::initScissor(command_buffers_[i], width, height);

                    command_buffers_[i].bindPipeline(vk::PipelineBindPoint::eGraphics,
                                                     program_obj->graphic_pipeline->getPipeline());

                    command_buffers_[i].bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                                           program_obj->descriptor_set->getPipelineLayout(), 0,
                                                           {data->descriptor_set}, {});

                    command_buffers_[i].bindVertexBuffers(0, {data->vertex_buffer->getVertexBuffer()->buf}, {0});

                    auto index_count = data->vertex_buffer->getIndexSize();
                    if(index_count > 0) {
                        command_buffers_[i].bindIndexBuffer(data->vertex_buffer->getIndexBuffer()->buf, 0, vk::IndexType::eUint32);
                        command_buffers_[i].drawIndexed(index_count, 1, 0, 0, 1);
                    } else {
                        command_buffers_[i].draw(data->vertex_buffer->getVertexSize(), 1, 0, 0);
                    }
                }
            }

            command_buffers_[i].endRenderPass();

            command_buffers_[i].end();
        }
    }

    std::vector<vk::CommandBuffer> CommandBuffers::getCommandBuffers()
    {
        return command_buffers_;
    }

}
