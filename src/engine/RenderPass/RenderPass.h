//
// Created by luizorv on 9/3/17.
//

#ifndef OBSIDIAN2D_RENDERPASS_H
#define OBSIDIAN2D_RENDERPASS_H

#include <array>
#include <vulkan/vulkan.hpp>

namespace Engine
{
	namespace RenderPass
	{
		struct RpAttachments
		{
			vk::Format format{};
			vk::ImageUsageFlags usage{};
			vk::ImageLayout final_layout{};
			bool clear = true;
		};

		class RenderPass {

		private:

			vk::RenderPass render_pass_{};

		public:

            explicit RenderPass(std::vector<RpAttachments> att_vector);

			~RenderPass();

			vk::RenderPass getRenderPass() const;
		};
	}
}

#endif //OBSIDIAN2D_RENDERPASS_H
