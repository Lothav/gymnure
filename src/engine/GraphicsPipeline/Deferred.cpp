#include "Deferred.hpp"
namespace Engine
{
    namespace GraphicsPipeline
    {
        Deferred::Deferred()
        {
            auto app_data = ApplicationData::data;

            {
                Memory::ImageProps img_props = {};
                img_props.width             = static_cast<uint32_t>(app_data->view_width);
                img_props.height            = static_cast<uint32_t>(app_data->view_height);
                img_props.format            = vk::Format::eR8G8B8A8Unorm;
                img_props.usage             =                 // Image will be
                    vk::ImageUsageFlagBits::eSampled |        // sampled in shaders and
                    vk::ImageUsageFlagBits::eColorAttachment; // used as Framebuffer attachment
                img_props.tiling            = vk::ImageTiling::eOptimal;
                img_props.image_props_flags = vk::MemoryPropertyFlagBits::eDeviceLocal;

                auto albedo_buffer_img = std::make_unique<Memory::BufferImage>(img_props);
                g_buffer_.albedo = std::make_unique<Descriptors::Texture>(std::move(albedo_buffer_img));
            }

            // Create Render Pass
            {
                std::vector<RenderPass::RpAttachments> rp_attachments = {};
                RenderPass::RpAttachments attch = {};
                attch.format        = Memory::ImageFormats::getSurfaceFormat().format;
                attch.final_layout  = vk::ImageLayout::eShaderReadOnlyOptimal;
                attch.usage         = vk::ImageUsageFlagBits::eColorAttachment;
                rp_attachments.push_back(attch);

                render_pass_ = std::make_shared<RenderPass::RenderPass>(rp_attachments);
                command_buffers_.push_back(std::make_unique<CommandBuffer>());
            }

            // Create Frame Buffers
            {
                std::vector<vk::ImageView> img_attachments = { g_buffer_.albedo->getImageView() };
                frame_buffers_.push_back(std::make_shared<RenderPass::FrameBuffer>(img_attachments, render_pass_));
            }
        }

        vk::RenderPass Deferred::getRenderPass() const
        {
            return render_pass_->getRenderPass();
        }

        void Deferred::prepare(const std::vector<std::shared_ptr<Programs::Program>>& programs)
        {
            std::vector<vk::ClearValue> clear_values = {
                vk::ClearValue{ vk::ClearColorValue(std::array<float, 4>({ 0.4f, 0.4f, 0.4f, 1.0f })) }
            };

            uint32_t i = 0;
            for (auto& command_buffer: command_buffers_)
            {
                command_buffer->bindGraphicCommandBuffer(clear_values, render_pass_, frame_buffers_[i++], programs);
            }
        }

        void Deferred::render()
        {
            vk::Queue queue = RenderPass::SwapChain::getInstance()->getGraphicQueue();

            vk::CommandBuffer current_command_buffer = command_buffers_[0]->getCommandBuffer();
            vk::PipelineStageFlags pipe_stage_flags = vk::PipelineStageFlagBits::eColorAttachmentOutput;

            vk::SubmitInfo submit_info = {};
            submit_info.pNext                     = nullptr;
            submit_info.waitSemaphoreCount        = 1;
            submit_info.pWaitDstStageMask         = &pipe_stage_flags;
            submit_info.commandBufferCount        = 1;
            submit_info.pCommandBuffers           = &current_command_buffer;
            submit_info.signalSemaphoreCount      = 1;
            submit_info.pWaitSemaphores           = nullptr;
            submit_info.pSignalSemaphores         = nullptr;

            DEBUG_CALL(queue.submit({submit_info}, nullptr));
        }
    }
}