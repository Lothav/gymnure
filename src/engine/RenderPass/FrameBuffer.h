//
// Created by luizorv on 9/3/17.
//

#ifndef OBSIDIAN2D_FRAMEBUFFER_H
#define OBSIDIAN2D_FRAMEBUFFER_H

#include <memory>
#include "RenderPass/Swapchain.h"
#include "RenderPass.h"

namespace Engine
{
	namespace RenderPass
	{
		class FrameBuffer {

		private:

			std::vector<vk::Framebuffer>  		 frame_buffers_;

			std::shared_ptr<SwapChain> 			 swap_chain_;
			std::shared_ptr<RenderPass>			 render_pass_;
			std::unique_ptr<Memory::BufferImage> depth_buffer_;
			vk::Format 					 		 depth_format_ = vk::Format::eUndefined;

		public:

			FrameBuffer();

			virtual ~FrameBuffer();

			uint32_t getImageCount() const;
			const vk::Format getDepthBufferFormat() const;
			std::shared_ptr<SwapChain> getSwapChain() const;
			vk::RenderPass getRenderPass() const
			{
				return render_pass_->getRenderPass();
			}
			std::vector<vk::Framebuffer> getFrameBuffers() const;

		};
	}
}

#endif //OBSIDIAN2D_FRAMEBUFFER_H
