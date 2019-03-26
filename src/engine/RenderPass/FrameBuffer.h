//
// Created by luizorv on 9/3/17.
//

#ifndef OBSIDIAN2D_FRAMEBUFFER_H
#define OBSIDIAN2D_FRAMEBUFFER_H

#include <memory>
#include "RenderPass/Swapchain.h"

namespace Engine
{
	namespace RenderPass
	{
		class FrameBuffer {

		private:

			std::unique_ptr<Memory::BufferImage> depth_buffer_;
			std::shared_ptr<SwapChain> 			 swap_chain_;

			std::vector<vk::Framebuffer>  		 frame_buffers_;
			vk::Format 					 		 depth_format_ = vk::Format::eUndefined;

		public:

			FrameBuffer();

			virtual ~FrameBuffer();

			void* operator new(std::size_t size)
			{
				return mem::Provider::getMemory(size);
			}

			void operator delete(void* ptr)
			{
				// Do not free memory here!
			}

			void createFrameBuffer(vk::RenderPass render_pass);

			const vk::Format getDepthBufferFormat() const;

			std::shared_ptr<SwapChain> getSwapChain();

			std::vector<vk::Framebuffer> getFrameBuffers();

		};
	}
}

#endif //OBSIDIAN2D_FRAMEBUFFER_H
