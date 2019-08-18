//
// Created by luizorv on 9/3/17.
//

#ifndef OBSIDIAN2D_SWAPCHAIN_H
#define OBSIDIAN2D_SWAPCHAIN_H

#include <vector>
#include <iostream>
#include <vulkan/vulkan.hpp>
#include "Descriptors/Texture.hpp"
#include "Memory/BufferImage.h"
#include <Util/Debug.hpp>

namespace Engine
{
	namespace RenderPass
	{
		class SwapChain {

		private:

			uint32_t 											image_count_{};
			vk::SwapchainKHR  									swap_chain_ = nullptr;
			std::vector<std::unique_ptr<Memory::BufferImage>> 	swap_chain_buffer_ = {};

			static std::shared_ptr<SwapChain>                   instance;

			explicit SwapChain();

		public:

			static void reset();

			static std::shared_ptr<SwapChain> getInstance();

			uint32_t getImageCount() const;
			vk::ImageView getSwapChainImageView(uint32_t i) const;
			vk::SwapchainKHR getSwapChainKHR() const;

		private:

			vk::SwapchainCreateInfoKHR buildSwapChainCI();

		};
	}
}
#endif //OBSIDIAN2D_SWAPCHAIN_H
