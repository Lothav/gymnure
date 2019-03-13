//
// Created by luizorv on 9/3/17.
//

#ifndef OBSIDIAN2D_FRAMEBUFFER_H
#define OBSIDIAN2D_FRAMEBUFFER_H

#include <Application.hpp>
#include "RenderPass/Swapchain.h"

namespace Engine
{
	namespace RenderPass
	{
		class FrameBuffer {

		protected:

			SwapChain* 					_swap_chain = nullptr;

		private:

			Memory::BufferImage* 		_depth_buffer{};
			std::vector<VkFramebuffer>  _frame_buffers;
			VkFormat 					_depth_format;

		public:

			FrameBuffer()
			{
				_swap_chain = new SwapChain();

				createDepthBuffer();
			}

			virtual ~FrameBuffer()
			{
				delete _depth_buffer;
				delete _swap_chain;
				for (auto &_frame_buffer : _frame_buffers)
					vkDestroyFramebuffer(ApplicationData::data->device, _frame_buffer, nullptr);
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
				img_attachments[1] = _depth_buffer->view;

				VkFramebufferCreateInfo fb_info = {};
				fb_info.sType 					= VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				fb_info.pNext 					= nullptr;
				fb_info.renderPass 				= render_pass;
				fb_info.attachmentCount 		= 2;
				fb_info.pAttachments 			= img_attachments;
				fb_info.width 					= app_data->view_width;
				fb_info.height 					= app_data->view_height;
				fb_info.layers 					= 1;

				_frame_buffers.resize(_swap_chain->getImageCount());

				for (uint32_t i = 0; i < _swap_chain->getImageCount(); i++) {
					img_attachments[0] = (_swap_chain->getSwapChainBuffer(i))->view;
					assert(vkCreateFramebuffer(app_data->device, &fb_info, nullptr, &_frame_buffers[i]) == VK_SUCCESS);
				}
			}

			VkFormat getDepthBufferFormat()
			{
				return _depth_format;
			}

			SwapChain* getSwapChain()
			{
				return _swap_chain;
			}

			std::vector<VkFramebuffer> getFrameBuffers()
			{
				return _frame_buffers;
			}

		private :

			void createDepthBuffer()
			{
				auto app_data = ApplicationData::data;

				_depth_format = VK_FORMAT_D16_UNORM;

				VkFormatProperties props;
				VkImageTiling depth_tiling;

				vkGetPhysicalDeviceFormatProperties(app_data->gpu, _depth_format, &props);

				if (props.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
					depth_tiling = VK_IMAGE_TILING_LINEAR;
				} else if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
					depth_tiling = VK_IMAGE_TILING_OPTIMAL;
				} else {
					/* Try other depth formats? */
					std::cerr << "depth_format " << _depth_format << " Unsupported.\n";
					assert(false);
				}

				struct ImageProps img_props = {};
				img_props.format 		= _depth_format;
				img_props.tiling 		= depth_tiling;
				img_props.aspectMask 	= VK_IMAGE_ASPECT_DEPTH_BIT;
				img_props.usage 		= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
				img_props.width 		= app_data->view_width;
				img_props.height 		= app_data->view_height;

				struct MemoryProps mem_props = {};
				mem_props.memory_props 	= app_data->memory_properties;

				_depth_buffer = new Memory::BufferImage(mem_props, img_props);
			}

		};
	}
}

#endif //OBSIDIAN2D_FRAMEBUFFER_H
