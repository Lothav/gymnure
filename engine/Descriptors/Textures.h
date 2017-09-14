//
// Created by luizorv on 8/25/17.
//

#ifndef OBSIDIAN2D_TEXTURES_H
#define OBSIDIAN2D_TEXTURES_H

#include <vector>
#include "Memory/Memory.h"
#include "Memory/Buffer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

namespace Engine
{
	namespace Descriptors
	{
		class Textures
		{

		private:

			static void createImage(VkDevice device, uint32_t width, uint32_t height, VkFormat format,
								  	VkImageTiling tiling, VkImageUsageFlags usage,
									VkMemoryPropertyFlags properties, VkImage& image,
                                    VkPhysicalDeviceMemoryProperties memory_properties)
			{
				VkResult res;

				VkImageCreateInfo imageInfo = {};
				imageInfo.sType 					 = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
				imageInfo.imageType 				 = VK_IMAGE_TYPE_2D;
				imageInfo.extent.width 				 = width;
				imageInfo.extent.height 			 = height;
				imageInfo.extent.depth 				 = 1;
				imageInfo.mipLevels 				 = 1;
				imageInfo.arrayLayers 				 = 1;
				imageInfo.format 					 = format;
				imageInfo.tiling 					 = tiling;
				imageInfo.initialLayout 			 = VK_IMAGE_LAYOUT_UNDEFINED;
				imageInfo.usage 					 = usage;
				imageInfo.samples 					 = VK_SAMPLE_COUNT_1_BIT;
				imageInfo.sharingMode 				 = VK_SHARING_MODE_EXCLUSIVE;

				res = vkCreateImage(device, &imageInfo, nullptr, &image);
				assert(res == VK_SUCCESS);

				VkMemoryRequirements memRequirements;
				vkGetImageMemoryRequirements(device, image, &memRequirements);

				VkMemoryAllocateInfo allocInfo = {};
				allocInfo.sType 					= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				allocInfo.allocationSize 			= memRequirements.size;

				Memory::Memory::findMemoryType(
						memory_properties,
						memRequirements.memoryTypeBits,
						properties,
						&allocInfo.memoryTypeIndex
				);

                textureImageMemory.resize(textureImageMemory.size() + 1);
				res = vkAllocateMemory(device, &allocInfo, nullptr, &textureImageMemory[ textureImageMemory.size()-1 ]);
				assert(res == VK_SUCCESS);

				vkBindImageMemory(device, image, textureImageMemory[ textureImageMemory.size()-1 ], 0);
			}

			static VkCommandBuffer beginSingleTimeCommands(VkCommandPool commandPool, VkDevice device)
			{
				VkCommandBufferAllocateInfo allocInfo = {};
				allocInfo.sType 							= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				allocInfo.level 							= VK_COMMAND_BUFFER_LEVEL_PRIMARY;
				allocInfo.commandPool 						= commandPool;
				allocInfo.commandBufferCount 				= 1;

				VkCommandBuffer commandBuffer;
				vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

				VkCommandBufferBeginInfo beginInfo = {};
				beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

				vkBeginCommandBuffer(commandBuffer, &beginInfo);

				return commandBuffer;
			}

			static void endSingleTimeCommands(
					VkCommandBuffer commandBuffer,
					VkCommandPool commandPool,
					VkQueue graphicsQueue,
					VkDevice device)
			{
				vkEndCommandBuffer(commandBuffer);

				VkSubmitInfo submitInfo = {};
				submitInfo.sType 							= VK_STRUCTURE_TYPE_SUBMIT_INFO;
				submitInfo.commandBufferCount 				= 1;
				submitInfo.pCommandBuffers 					= &commandBuffer;

				vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
				vkQueueWaitIdle(graphicsQueue);

				vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
			}

			static void transitionImageLayout(
					VkImage image,
					VkFormat format,
					VkImageLayout oldLayout,
					VkImageLayout newLayout,
					VkCommandPool commandPool,
					VkQueue graphicsQueue,
					VkDevice device)
			{
				VkCommandBuffer commandBuffer = beginSingleTimeCommands(commandPool, device);

				VkImageMemoryBarrier barrier = {};
				barrier.sType 								= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				barrier.oldLayout 							= oldLayout;
				barrier.newLayout 							= newLayout;
				barrier.srcQueueFamilyIndex 				= VK_QUEUE_FAMILY_IGNORED;
				barrier.dstQueueFamilyIndex 				= VK_QUEUE_FAMILY_IGNORED;
				barrier.image 								= image;
				barrier.subresourceRange.aspectMask 		= VK_IMAGE_ASPECT_COLOR_BIT;
				barrier.subresourceRange.baseMipLevel 		= 0;
				barrier.subresourceRange.levelCount 		= 1;
				barrier.subresourceRange.baseArrayLayer 	= 0;
				barrier.subresourceRange.layerCount 		= 1;

				VkPipelineStageFlags sourceStage;
				VkPipelineStageFlags destinationStage;

				if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
					barrier.srcAccessMask 					= 0;
					barrier.dstAccessMask 					= VK_ACCESS_TRANSFER_WRITE_BIT;
					sourceStage 							= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
					destinationStage 						= VK_PIPELINE_STAGE_TRANSFER_BIT;
				} else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
					barrier.srcAccessMask 					= VK_ACCESS_TRANSFER_WRITE_BIT;
					barrier.dstAccessMask 					= VK_ACCESS_SHADER_READ_BIT;

					sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
					destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				} else {
					return;
				}

				vkCmdPipelineBarrier ( commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
				endSingleTimeCommands(commandBuffer, commandPool, graphicsQueue, device);
			}

			static void copyBufferToImage (
					VkBuffer buffer,
					VkImage image,
					uint32_t width,
					uint32_t height,
					VkCommandPool commandPool,
					VkQueue graphicsQueue,
					VkDevice device)
			{
				VkCommandBuffer commandBuffer = beginSingleTimeCommands(commandPool, device);

				VkBufferImageCopy region = {};
				region.bufferOffset 						= 0;
				region.bufferRowLength 						= 0;
				region.bufferImageHeight 					= 0;
				region.imageSubresource.aspectMask 			= VK_IMAGE_ASPECT_COLOR_BIT;
				region.imageSubresource.mipLevel 			= 0;
				region.imageSubresource.baseArrayLayer 		= 0;
				region.imageSubresource.layerCount 			= 1;
				region.imageOffset 							= {0, 0, 0};
				region.imageExtent 							= {width, height, 1};

				vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
				endSingleTimeCommands(commandBuffer, commandPool, graphicsQueue, device);
			}

		public:

			static std::vector<VkDeviceMemory> textureImageMemory ;

			static VkImage createTextureImage(
					VkPhysicalDevice physicalDevice,
					VkDevice device,
					const char* path,
					VkCommandPool commandPool,
					VkQueue graphicQueue,
					VkPhysicalDeviceMemoryProperties memory_properties)
			{
				VkResult 				res;
				VkImage 				textureImage;

				int texWidth, texHeight, texChannels;
				stbi_uc* pixels = stbi_load(path, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
				VkDeviceSize imageSize = static_cast<VkDeviceSize>(texWidth * texHeight * 4);

				assert(pixels);

				struct BufferData stagingBufferData = {};

				stagingBufferData.device            = device;
				stagingBufferData.usage             = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
				stagingBufferData.physicalDevice    = physicalDevice;
				stagingBufferData.properties        = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
				stagingBufferData.size              = imageSize;

				Memory::Buffer * staging_buffer = new Memory::Buffer(stagingBufferData);

				void* data = nullptr;
				vkMapMemory(device, staging_buffer->mem, 0, imageSize, 0, &data);
				memcpy(data, pixels, static_cast<size_t>(imageSize));
				vkUnmapMemory(device, staging_buffer->mem);

				stbi_image_free(pixels);

				createImage(device, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight),
							VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
							VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
							VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, memory_properties);

				transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, commandPool, graphicQueue, device);
				copyBufferToImage(staging_buffer->buf, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), commandPool, graphicQueue, device);
				transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, commandPool, graphicQueue, device);

				delete staging_buffer;

				return textureImage;
			}

		};

		std::vector<VkDeviceMemory> Textures::textureImageMemory;
	}
}

#endif //OBSIDIAN2D_TEXTURES_H
