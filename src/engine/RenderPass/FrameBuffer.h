//
// Created by luizorv on 9/3/17.
//

#ifndef OBSIDIAN2D_FRAMEBUFFER_H
#define OBSIDIAN2D_FRAMEBUFFER_H

#include <memory>
#include <Application.hpp>
#include "RenderPass/Swapchain.h"

namespace Engine
{
	namespace RenderPass
	{
		class FrameBuffer {

		private:

			std::unique_ptr<Memory::BufferImage> depth_buffer_;
			std::shared_ptr<SwapChain> 			 swap_chain_;

			std::vector<VkFramebuffer>  		 frame_buffers_;
			const VkFormat 						 depth_format_ = VK_FORMAT_D32_SFLOAT_S8_UINT;

		public:

			FrameBuffer()
			{
				swap_chain_ = std::make_shared<SwapChain>();

				auto app_data = ApplicationData::data;

				VkFormatProperties props;
				VkImageTiling depth_tiling;

				vkGetPhysicalDeviceFormatProperties(app_data->gpu, depth_format_, &props);

				if (props.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
					depth_tiling = VK_IMAGE_TILING_LINEAR;
				} else if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
					depth_tiling = VK_IMAGE_TILING_OPTIMAL;
				} else {
					/* Try other depth formats? */
					std::cerr << "depth_format " << depth_format_ << " Unsupported.\n";
					assert(false);
				}

				struct ImageProps img_props = {};
				img_props.format 		= depth_format_;
				img_props.tiling 		= depth_tiling;
				img_props.aspectMask 	= VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
				img_props.usage 		= VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
				img_props.width 		= app_data->view_width;
				img_props.height 		= app_data->view_height;

				struct MemoryProps mem_props = {};
				mem_props.props_flags   = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

				// Create Depth Buffer
				depth_buffer_ = std::make_unique<Memory::BufferImage>(mem_props, img_props);
			}

			virtual ~FrameBuffer()
			{
				for (auto &frame_buffer_ : frame_buffers_)
					vkDestroyFramebuffer(ApplicationData::data->device, frame_buffer_, nullptr);
			}

			void* operator new(std::size_t size)
			{
				return mem::Provider::getMemory(size);
			}

			void operator delete(void* ptr)
			{
				// Do not free memory here!
			}

			void createFrameBuffer(VkRenderPass render_pass)
			{
				auto app_data = ApplicationData::data;

				VkImageView img_attachments[2];
				img_attachments[1] = depth_buffer_->view;

				VkFramebufferCreateInfo fb_info = {};
				fb_info.sType 					= VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				fb_info.pNext 					= nullptr;
				fb_info.renderPass 				= render_pass;
				fb_info.attachmentCount 		= 2;
				fb_info.pAttachments 			= img_attachments;
				fb_info.width 					= app_data->view_width;
				fb_info.height 					= app_data->view_height;
				fb_info.layers 					= 1;

				frame_buffers_.resize(swap_chain_->getImageCount());

				for (uint32_t i = 0; i < swap_chain_->getImageCount(); i++) {
					img_attachments[0] = (swap_chain_->getSwapChainBuffer(i))->view;
					assert(vkCreateFramebuffer(app_data->device, &fb_info, nullptr, &frame_buffers_[i]) == VK_SUCCESS);
				}
			}

			const VkFormat getDepthBufferFormat() const
			{
				return depth_format_;
			}

			std::shared_ptr<SwapChain> getSwapChain()
			{
				return swap_chain_;
			}

			std::vector<VkFramebuffer> getFrameBuffers()
			{
				return frame_buffers_;
			}

		};
	}
}

#endif //OBSIDIAN2D_FRAMEBUFFER_H
