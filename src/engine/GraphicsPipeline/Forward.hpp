#ifndef GYMNURE_FORWARD_HPP
#define GYMNURE_FORWARD_HPP

#include <vulkan/vulkan.hpp>
#include <Memory/BufferImage.h>
#include <RenderPass/RenderPass.h>
#include <Memory/ImageFormats.hpp>
#include <RenderPass/FrameBuffer.h>
#include <SyncPrimitives/SyncPrimitives.h>
#include <CommandBuffer.h>

namespace Engine
{
	namespace GraphicsPipeline
	{
		class Forward
		{

		private:

			std::shared_ptr<RenderPass::SwapChain> 					swap_chain_ 	 = nullptr;
			std::unique_ptr<Memory::BufferImage> 					depth_buffer_ 	 = nullptr;

			std::shared_ptr<RenderPass::RenderPass> 				render_pass_ 	 = nullptr;
			std::unique_ptr<SyncPrimitives::SyncPrimitives> 	    sync_primitives_ = nullptr;

			std::vector<std::shared_ptr<RenderPass::FrameBuffer>> 	frame_buffers_ 	 = {};
			std::vector<std::unique_ptr<CommandBuffer>>            command_buffers_  = {};

			uint32_t 												current_buffer_  = 0;

		public:

			Forward() : swap_chain_(std::make_shared<RenderPass::SwapChain>())
			{
				auto app_data = ApplicationData::data;

				// Create Depth Buffer
				{
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

					attch.format        = Memory::ImageFormats::getImageFormat(Memory::ImageType::DEPTH_STENCIL);
					attch.final_layout  = vk::ImageLayout::eDepthStencilAttachmentOptimal;
					attch.usage         = vk::ImageUsageFlagBits::eDepthStencilAttachment;
					rp_attachments.push_back(attch);

					render_pass_ = std::make_shared<RenderPass::RenderPass>(rp_attachments);
				}

				{
					std::vector<vk::ImageView> img_attachments = {};
					img_attachments.resize(2);
					img_attachments[1] = depth_buffer_->view;

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

			~Forward()
			{

			}

			vk::RenderPass getRenderPass() const
			{
				return render_pass_->getRenderPass();
			}

			void prepare(const std::vector<std::shared_ptr<Programs::Program>>& programs)
			{
				std::vector<vk::ClearValue> clear_values = {};
				clear_values.resize(2);
				clear_values[0].color        = vk::ClearColorValue(std::array<float, 4>({ 0.4f, 0.4f, 0.4f, 1.0f }));
				clear_values[1].depthStencil = vk::ClearDepthStencilValue{1.0f, 0u};

				uint32_t i = 0;
				for (auto& command_buffer: command_buffers_)
				{
					command_buffer->bindGraphicCommandBuffer(clear_values, render_pass_, frame_buffers_[i++], programs);
				}
			}

			void render()
			{
				vk::Result res = vk::Result::eNotReady;

				auto device = ApplicationData::data->device;
				auto swapchainKHR = swap_chain_->getSwapChainKHR();

				DEBUG_CALL(
					std::tie(res, current_buffer_) = device.acquireNextImageKHR(
						swapchainKHR, UINT64_MAX, sync_primitives_->imageAcquiredSemaphore, {}));
				assert(res == vk::Result::eSuccess);

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

				DEBUG_CALL(swap_chain_->getGraphicQueue().submit({submit_info}, current_buffer_fence));

				vk::PresentInfoKHR present = {};
				present.pNext 				  = nullptr;
				present.swapchainCount 		  = 1;
				present.pSwapchains 		  = &swapchainKHR;
				present.pImageIndices 		  = &current_buffer_;
				present.pWaitSemaphores 	  = nullptr;
				present.waitSemaphoreCount 	  = 0;
				present.pResults              = nullptr;

				if (sync_primitives_->renderSemaphore) {
					present.pWaitSemaphores = &sync_primitives_->renderSemaphore;
					present.waitSemaphoreCount = 1;
				}

				DEBUG_CALL(swap_chain_->getGraphicQueue().presentKHR(&present));
			}
		};
	}
}

#endif //GYMNURE_FORWARD_HPP
