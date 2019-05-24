#include <RenderPass/FrameBuffer.h>
#include "CommandBuffer.h"

namespace Engine
{
    CommandBuffer::CommandBuffer()
    {
        auto app_data = ApplicationData::data;

        vk::CommandBufferAllocateInfo cmd_buff_ai = {};
        cmd_buff_ai.pNext 			 	= nullptr;
        cmd_buff_ai.commandPool 	 	= app_data->graphic_command_pool;
        cmd_buff_ai.level 			 	= vk::CommandBufferLevel::ePrimary;
        cmd_buff_ai.commandBufferCount  = 1;

        command_buffer_ = app_data->device.allocateCommandBuffers(cmd_buff_ai)[0];
    }

    CommandBuffer::~CommandBuffer()
    {
        auto app_data = ApplicationData::data;
        app_data->device.freeCommandBuffers(app_data->graphic_command_pool, {command_buffer_});
    }

    void CommandBuffer::bindGraphicCommandBuffer(
        std::vector<vk::ClearValue> clear_values,
        std::shared_ptr<RenderPass::RenderPass> render_pass,
        std::shared_ptr<RenderPass::FrameBuffer> frame_buffer,
        std::vector<std::shared_ptr<Programs::Program>> programs)
    {
        uint32_t width = ApplicationData::data->view_width;
        uint32_t height = ApplicationData::data->view_height;

        vk::CommandBufferBeginInfo cmd_buf_info = {};
        cmd_buf_info.pNext 				= nullptr;
        cmd_buf_info.flags 				= vk::CommandBufferUsageFlagBits::eSimultaneousUse;
        cmd_buf_info.pInheritanceInfo 	= nullptr;

        vk::RenderPassBeginInfo rp_begin = {};
        rp_begin.pNext 				= nullptr;
        rp_begin.renderPass 		= render_pass->getRenderPass();
        rp_begin.renderArea.offset 	= vk::Offset2D{0, 0};
        rp_begin.renderArea.extent  = vk::Extent2D{width, height};
        rp_begin.clearValueCount 	= static_cast<uint32_t>(clear_values.size());
        rp_begin.pClearValues 		= clear_values.data();

        size_t dynamicAlignment = Memory::Memory::getDynamicAlignment<glm::mat4>();

        command_buffer_.begin(cmd_buf_info);

        rp_begin.setFramebuffer(frame_buffer->getFrameBufferKHR());
        command_buffer_.beginRenderPass(rp_begin, vk::SubpassContents::eInline);

        for(auto& program_obj : programs)
        {
            uint32_t j = 0;
            for(auto &data : program_obj->data)
            {
                Util::Util::initViewport(command_buffer_, width, height);
                Util::Util::initScissor(command_buffer_, width, height);

                command_buffer_.bindPipeline(vk::PipelineBindPoint::eGraphics, program_obj->graphic_pipeline->getPipeline());
                uint32_t dynamicOffset = j * static_cast<uint32_t>(dynamicAlignment);

                command_buffer_.bindDescriptorSets(
                    vk::PipelineBindPoint::eGraphics,
                    program_obj->descriptor_set->getPipelineLayout(), 0,
                    {data->descriptor_set}, {dynamicOffset});

                command_buffer_.bindVertexBuffers(0, {data->vertex_buffer->getVertexBuffer()}, {0});

                auto index_count = data->vertex_buffer->getIndexCount();
                if(index_count > 0) {
                    command_buffer_.bindIndexBuffer(data->vertex_buffer->getIndexBuffer(), 0, vk::IndexType::eUint32);
                    command_buffer_.drawIndexed(index_count, 1, 0, 0, 0);
                } else {
                    command_buffer_.draw(data->vertex_buffer->getVertexCount(), 1, 0, 0);
                }

                j++;
            }
        }

        command_buffer_.endRenderPass();
        command_buffer_.end();
    }

    vk::CommandBuffer CommandBuffer::getCommandBuffer() const
    {
        return command_buffer_;
    }

}
