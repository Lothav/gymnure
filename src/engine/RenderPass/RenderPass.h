//
// Created by luizorv on 9/3/17.
//

#ifndef OBSIDIAN2D_RENDERPASS_H
#define OBSIDIAN2D_RENDERPASS_H

#include <array>
#include <vulkan/vulkan.hpp>

struct rpAttachments
{
	vk::Format format;
	bool clear;
};

namespace Engine
{
	namespace RenderPass
	{
		class RenderPass {

		private:

			vk::RenderPass render_pass_{};

		public:

            RenderPass(std::vector<struct rpAttachments> att_vector);

			~RenderPass();

			vk::RenderPass getRenderPass() const;
		};
	}
}

#endif //OBSIDIAN2D_RENDERPASS_H
