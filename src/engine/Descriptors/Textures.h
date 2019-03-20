//
// Created by luizorv on 8/25/17.
//

#ifndef OBSIDIAN2D_TEXTURES_H
#define OBSIDIAN2D_TEXTURES_H
#include "vulkan/vulkan.h"

#include <vector>
#include <memancpp/Allocator.hpp>
#include "Memory/Memory.h"
#include "Memory/Buffer.h"

namespace Engine
{
	namespace Descriptors
	{
		class Textures
		{

		public:

			static std::vector<vk::DeviceMemory, mem::StdAllocator<vk::DeviceMemory>> textureImageMemory;
			static vk::Image createTextureImage(const std::string &texture_path, vk::Queue graphicQueue);

		private:

            static void createImage(uint32_t width, uint32_t height, vk::Format format,
									vk::ImageTiling tiling, vk::ImageUsageFlags usage,
									vk::MemoryPropertyFlags properties, vk::Image& image);
			static vk::CommandBuffer beginSingleTimeCommands();
			static void endSingleTimeCommands(vk::CommandBuffer commandBuffer, vk::Queue graphicsQueue);
			static void transitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::Queue graphicsQueue);
			static void copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height, vk::Queue graphicsQueue);

		};
	}
}

#endif //OBSIDIAN2D_TEXTURES_H
