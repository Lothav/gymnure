//
// Created by luizorv on 9/3/17.
//

#ifndef OBSIDIAN2D_SWAPCHAIN_H
#define OBSIDIAN2D_SWAPCHAIN_H

#include <vulkan/vulkan.hpp>
#include <iostream>
#include "Descriptors/Textures.h"
#include "Memory/BufferImage.h"
#include <vector>
#include <Util/Debug.hpp>

namespace Engine
{
	namespace RenderPass
	{
		class SwapChain {

		private:

			uint32_t 							image_count_{};
			vk::SwapchainKHR  					swap_chain_ = nullptr;
			vk::Queue 							graphics_queue_{}, present_queue_{};
			vk::Format 							format_;
			vk::ColorSpaceKHR						colorSpace_;

			std::vector<Memory::BufferImage *> 	swap_chain_buffer_ = {};

		public:

			explicit SwapChain();

			~SwapChain();

			void* operator new(std::size_t size)
			{
				return mem::Provider::getMemory(size);
			}

			void operator delete(void* ptr)
			{
				// Do not free memory here!
			}

			uint32_t getImageCount() const;

			Memory::BufferImage * getSwapChainBuffer(uint32_t i) const;

			vk::Format getSwapChainFormat() const;

			vk::SwapchainKHR getSwapChainKHR() const;

			vk::Queue getGraphicQueue() const;

			vk::Queue getPresentQueue() const;

		private:

			vk::SwapchainCreateInfoKHR buildSwapChainCI();

		};
	}
}
#endif //OBSIDIAN2D_SWAPCHAIN_H
