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

			vk::Framebuffer frame_buffer_;

		public:

			FrameBuffer(std::vector<vk::ImageView> img_attachments, std::shared_ptr<RenderPass> render_pass);
			virtual ~FrameBuffer();

			vk::Framebuffer getFrameBufferKHR() const;

		};
	}
}

#endif //OBSIDIAN2D_FRAMEBUFFER_H
