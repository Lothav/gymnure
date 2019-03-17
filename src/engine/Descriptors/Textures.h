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

			static std::vector<VkDeviceMemory, mem::StdAllocator<VkDeviceMemory>> textureImageMemory;
			static VkImage createTextureImage(const std::string &texture_path, VkQueue graphicQueue);

		private:

            static void createImage(uint32_t width, uint32_t height, VkFormat format,
                                    VkImageTiling tiling, VkImageUsageFlags usage,
                                    VkMemoryPropertyFlags properties, VkImage& image);
			static VkCommandBuffer beginSingleTimeCommands();
			static void endSingleTimeCommands(VkCommandBuffer commandBuffer, VkQueue graphicsQueue);
			static void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkQueue graphicsQueue);
			static void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, VkQueue graphicsQueue);

		};
	}
}

#endif //OBSIDIAN2D_TEXTURES_H
