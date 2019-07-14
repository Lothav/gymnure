
#include "Pipeline.hpp"

namespace Engine
{
    namespace GraphicsPipeline
    {
        Pipeline::Pipeline(const bool has_depth) : present_(true), color_targets_count_(1)
        {
            auto app_data = ApplicationData::data;
            std::shared_ptr<RenderPass::SwapChain> swap_chain_ = RenderPass::SwapChain::getInstance();

            if(has_depth)
            {
                // Create Depth Buffer
                Memory::ImageProps img_props = {};
                img_props.width             = static_cast<uint32_t>(app_data->view_width);
                img_props.height            = static_cast<uint32_t>(app_data->view_height);
                img_props.format            = Memory::ImageFormats::getImageFormat(Memory::ImageType::DEPTH_STENCIL);
                img_props.usage             = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eDepthStencilAttachment;
                img_props.tiling            = vk::ImageTiling::eOptimal;
                img_props.image_props_flags = vk::MemoryPropertyFlagBits::eDeviceLocal;

                // Create Depth Buffer
                depth_buffer_ = std::make_unique<Memory::BufferImage>(img_props);
            }

            // Create Render Pass
            {
                std::vector<RenderPass::RpAttachments> rp_attachments = {};

                RenderPass::RpAttachments attch = {};
                attch.format        = Memory::ImageFormats::getSurfaceFormat().format;
                attch.final_layout  = vk::ImageLayout::ePresentSrcKHR;
                attch.usage         = vk::ImageUsageFlagBits::eColorAttachment;
                rp_attachments.push_back(attch);

                if(has_depth)
                {
                    attch.format        = Memory::ImageFormats::getImageFormat(Memory::ImageType::DEPTH_STENCIL);
                    attch.final_layout  = vk::ImageLayout::eDepthStencilAttachmentOptimal;
                    attch.usage         = vk::ImageUsageFlagBits::eDepthStencilAttachment;
                    rp_attachments.push_back(attch);
                }

                render_pass_ = std::make_shared<RenderPass::RenderPass>(rp_attachments);
            }

            {
                std::vector<vk::ImageView> img_attachments = {};

                if(has_depth && depth_buffer_ != nullptr) {
                    img_attachments.resize(2);
                    img_attachments[1] = depth_buffer_->view;
                } else {
                    img_attachments.resize(1);
                }

                for (uint32_t j = 0; j < swap_chain_->getImageCount(); ++j)
                {
                    img_attachments[0] = swap_chain_->getSwapChainImageView(j);
                    frame_buffers_.push_back(std::make_shared<RenderPass::FrameBuffer>(img_attachments, render_pass_));
                    command_buffers_.push_back(std::make_unique<CommandBuffer>());
                }
            }

            // Init Sync Primitives
            sync_primitives_ = std::make_unique<SyncPrimitives::SyncPrimitives>();
            sync_primitives_->createSemaphore();
            sync_primitives_->createFences(swap_chain_->getImageCount());
        }

        // @TODO IMPLEMENT HAS_DEPTH
        Pipeline::Pipeline(const uint32_t color_targets_count, const bool has_depth) : present_(false), color_targets_count_(color_targets_count)
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

                for (uint32_t i = 0; i < color_targets_count_; ++i)
                {
                    auto albedo_buffer_img = std::make_unique<Memory::BufferImage>(img_props);
                    render_textures_.push_back(std::make_unique<Descriptors::Texture>(std::move(albedo_buffer_img)));
                }
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
            }

            // Create Frame Buffers
            {
                std::vector<vk::ImageView> img_attachments = {};
                for (int i = 0; i < color_targets_count_; ++i)
                    img_attachments.push_back(render_textures_[i]->getImageView());

                frame_buffers_.push_back(std::make_shared<RenderPass::FrameBuffer>(img_attachments, render_pass_));
                command_buffers_.push_back(std::make_unique<CommandBuffer>());
            }
        }

        vk::RenderPass Pipeline::getRenderPass() const
        {
            return render_pass_->getRenderPass();
        }

        void Pipeline::prepare(const std::vector<std::shared_ptr<Programs::Program>>& programs)
        {
            std::vector<vk::ClearValue> clear_values = {};

            for (int j = 0; j < color_targets_count_; ++j)
                clear_values.emplace_back(vk::ClearColorValue(std::array<float, 4>({ 0.4f, 0.4f, 0.4f, 1.0f })));

            if(depth_buffer_ != nullptr)
                clear_values.emplace_back(vk::ClearDepthStencilValue{1.0f, 0u});

            uint32_t i = 0;
            for (auto& command_buffer: command_buffers_)
                command_buffer->bindGraphicCommandBuffer(clear_values, render_pass_, frame_buffers_[i++], programs);
        }

        void Pipeline::render()
        {
            vk::Result res = vk::Result::eNotReady;

            auto device = ApplicationData::data->device;
            auto swapchain = RenderPass::SwapChain::getInstance();
            vk::Queue queue = swapchain->getGraphicQueue();

            if(present_)
            {
                DEBUG_CALL(
                    std::tie(res, current_buffer_) = device.acquireNextImageKHR(
                        swapchain->getSwapChainKHR(), UINT64_MAX,
                        sync_primitives_->imageAcquiredSemaphore, {}));
                assert(res == vk::Result::eSuccess);
            }

            vk::PipelineStageFlags pipe_stage_flags = vk::PipelineStageFlagBits::eColorAttachmentOutput;
            vk::CommandBuffer current_command_buffer = command_buffers_[current_buffer_]->getCommandBuffer();
            vk::Fence current_buffer_fence = sync_primitives_->getFence(current_buffer_);
            do {
                // Fences are created already signaled, so, we can wait for it before queue submit.
                res = device.waitForFences({current_buffer_fence}, VK_TRUE, UINT64_MAX);
            } while (res == vk::Result::eTimeout);
            DEBUG_CALL(device.resetFences({current_buffer_fence}));

            vk::SubmitInfo submit_info = {};
            submit_info.pNext                     = nullptr;
            submit_info.waitSemaphoreCount        = 1;
            submit_info.pWaitDstStageMask         = &pipe_stage_flags;
            submit_info.commandBufferCount        = 1;
            submit_info.pCommandBuffers           = &current_command_buffer;
            submit_info.signalSemaphoreCount      = 1;
            submit_info.pWaitSemaphores           = &sync_primitives_->imageAcquiredSemaphore;
            submit_info.pSignalSemaphores         = &sync_primitives_->renderSemaphore;

            DEBUG_CALL(queue.submit({submit_info}, current_buffer_fence));

            if(present_)
            {
                auto swapchainKHR = swapchain->getSwapChainKHR();

                vk::PresentInfoKHR present = {};
                present.pNext 				  = nullptr;
                present.swapchainCount 		  = 1;
                present.pSwapchains 		  = &swapchainKHR;
                present.pImageIndices 		  = &current_buffer_;
                present.pWaitSemaphores 	  = nullptr;
                present.waitSemaphoreCount 	  = 0;
                present.pResults              = nullptr;

                if (sync_primitives_->renderSemaphore)
                {
                    present.pWaitSemaphores = &sync_primitives_->renderSemaphore;
                    present.waitSemaphoreCount = 1;
                }

                DEBUG_CALL(queue.presentKHR(&present));
            }
        }
    }
}
