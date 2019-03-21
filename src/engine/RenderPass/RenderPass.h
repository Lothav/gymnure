//
// Created by luizorv on 9/3/17.
//

#ifndef OBSIDIAN2D_RENDERPASS_H
#define OBSIDIAN2D_RENDERPASS_H

#include <array>
#include "RenderPass/FrameBuffer.h"

struct rpAttachments
{
	vk::Format format;
	bool clear;
};

namespace Engine
{
	namespace RenderPass
	{
		class RenderPass : public FrameBuffer {

		private:

			vk::RenderPass _render_pass{};

		public:

			RenderPass() : FrameBuffer() {}

			~RenderPass();

			void* operator new(std::size_t size)
			{
				return mem::Provider::getMemory(size);
			}

			void operator delete(void* ptr)
			{
				// Do not free memory here!
			}

			void create(std::vector<struct rpAttachments> att_vector);

			vk::RenderPass getRenderPass() const;
		};
	}
}

#endif //OBSIDIAN2D_RENDERPASS_H
