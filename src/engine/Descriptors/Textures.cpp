#include <ApplicationData.hpp>
#include <Util/Util.h>
#include <memory>
#include "Textures.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

namespace Engine
{
    namespace Descriptors
    {
        std::vector<vk::DeviceMemory, mem::StdAllocator<vk::DeviceMemory>> Textures::textureImageMemory;

        vk::Image Textures::createTextureImage(const std::string &texture_path, vk::Queue graphicQueue)
        {
            auto app_data = ApplicationData::data;

            vk::Image textureImage;

            auto assets_texture_path = std::string(ASSETS_FOLDER_PATH_STR) + "/" + texture_path;

            int texWidth, texHeight, texChannels;
            stbi_uc *pixels = stbi_load(assets_texture_path.data(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
            auto imageSize = static_cast<vk::DeviceSize>(texWidth * texHeight * 4);

            assert(pixels);

            struct BufferData stagingBufferData = {};
            stagingBufferData.usage             = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            stagingBufferData.properties        = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            stagingBufferData.size              = imageSize;

            auto staging_buffer = std::make_unique<Memory::Buffer>(stagingBufferData);

            void *data = nullptr;
            vkMapMemory(app_data->device, staging_buffer->mem, 0, imageSize, 0, &data);
            memcpy(data, pixels, static_cast<size_t>(imageSize));
            vkUnmapMemory(app_data->device, staging_buffer->mem);

            stbi_image_free(pixels);

            createImage(static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight),
                        vk::Format::eR8G8B8A8Unorm, vk::ImageTiling::eOptimal,
                        vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
                        vk::MemoryPropertyFlagBits::eDeviceLocal, textureImage);

            transitionImageLayout(textureImage, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eUndefined,
                                  vk::ImageLayout::eTransferDstOptimal, graphicQueue);

            copyBufferToImage(staging_buffer->buf, textureImage, static_cast<uint32_t>(texWidth),
                              static_cast<uint32_t>(texHeight), graphicQueue);

            transitionImageLayout(textureImage, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eTransferDstOptimal,
                                  vk::ImageLayout::eShaderReadOnlyOptimal, graphicQueue);

            return textureImage;
        }

        void Textures::createImage(uint32_t width, uint32_t height, vk::Format format,
                                vk::ImageTiling tiling, vk::ImageUsageFlags usage,
                                vk::MemoryPropertyFlags properties, vk::Image& image)
        {
            auto app_data = ApplicationData::data;

            vk::Result res;

            vk::ImageCreateInfo imageInfo = {};
            imageInfo.imageType 		= vk::ImageType::e2D;
            imageInfo.extent.width 		= width;
            imageInfo.extent.height 	= height;
            imageInfo.extent.depth 		= 1;
            imageInfo.mipLevels 		= 1;
            imageInfo.arrayLayers 		= 1;
            imageInfo.format 			= format;
            imageInfo.tiling 			= tiling;
            imageInfo.initialLayout 	= vk::ImageLayout::eUndefined;
            imageInfo.usage 			= usage;
            imageInfo.samples 			= vk::SampleCountFlagBits::e1;
            imageInfo.sharingMode 		= vk::SharingMode::eExclusive;

            res = app_data->device.createImage(&imageInfo, nullptr, &image);
            assert(res == vk::Result::eSuccess);

            vk::MemoryRequirements memRequirements{};
            app_data->device.getImageMemoryRequirements(image, &memRequirements);

            vk::MemoryAllocateInfo allocInfo = {};
            allocInfo.allocationSize = memRequirements.size;

            Memory::Memory::findMemoryType(memRequirements.memoryTypeBits, properties, &allocInfo.memoryTypeIndex);

            textureImageMemory.resize(textureImageMemory.size() + 1);
            res = app_data->device.allocateMemory(&allocInfo, nullptr, &textureImageMemory[textureImageMemory.size() - 1]);
            assert(res == vk::Result::eSuccess);

            app_data->device.bindImageMemory(image, textureImageMemory[textureImageMemory.size() - 1], 0);
        }

        vk::CommandBuffer Textures::beginSingleTimeCommands()
        {
            auto app_data = ApplicationData::data;

            vk::CommandBufferAllocateInfo allocInfo = {};
            allocInfo.level 							= vk::CommandBufferLevel::ePrimary;
            allocInfo.commandPool 						= app_data->graphic_command_pool;
            allocInfo.commandBufferCount 				= 1;

            vk::CommandBuffer commandBuffer;
            app_data->device.allocateCommandBuffers(&allocInfo, &commandBuffer);

            vk::CommandBufferBeginInfo beginInfo = {};
            beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

            commandBuffer.begin(&beginInfo);

            return commandBuffer;
        }

        void Textures::endSingleTimeCommands(vk::CommandBuffer commandBuffer, vk::Queue graphicsQueue)
        {
            auto app_data = ApplicationData::data;

            commandBuffer.end();

            vk::SubmitInfo submitInfo = {};
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers 	  = &commandBuffer;

            graphicsQueue.submit(1, &submitInfo, nullptr);
            graphicsQueue.waitIdle();

            app_data->device.freeCommandBuffers(app_data->graphic_command_pool, 1, &commandBuffer);
        }

        void Textures::transitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::Queue graphicsQueue)
        {
            vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

            vk::ImageMemoryBarrier barrier = {};
            barrier.oldLayout 							= oldLayout;
            barrier.newLayout 							= newLayout;
            barrier.srcQueueFamilyIndex 				= VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex 				= VK_QUEUE_FAMILY_IGNORED;
            barrier.image 								= image;
            barrier.subresourceRange.aspectMask 		= vk::ImageAspectFlagBits::eColor;
            barrier.subresourceRange.baseMipLevel 		= 0;
            barrier.subresourceRange.levelCount 		= 1;
            barrier.subresourceRange.baseArrayLayer 	= 0;
            barrier.subresourceRange.layerCount 		= 1;

            vk::PipelineStageFlags sourceStage;
            vk::PipelineStageFlags destinationStage;

            if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal) {
                barrier.srcAccessMask 					= {};
                barrier.dstAccessMask 					= vk::AccessFlagBits::eTransferWrite;

                sourceStage 							= vk::PipelineStageFlagBits::eTopOfPipe;
                destinationStage 						= vk::PipelineStageFlagBits::eTransfer;
            } else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
                barrier.srcAccessMask 					= vk::AccessFlagBits::eTransferWrite;
                barrier.dstAccessMask 					= vk::AccessFlagBits::eShaderRead;

                sourceStage                             = vk::PipelineStageFlagBits::eTransfer;
                destinationStage                        = vk::PipelineStageFlagBits::eFragmentShader;
            } else {
                return;
            }

            commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, 0, nullptr, 0, nullptr, 1, &barrier);

            endSingleTimeCommands(commandBuffer, graphicsQueue);
        }

        void Textures::copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height, vk::Queue graphicsQueue)
        {
            vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

            vk::BufferImageCopy region = {};
            region.bufferOffset 					= 0;
            region.bufferRowLength 					= 0;
            region.bufferImageHeight 				= 0;
            region.imageOffset 						= vk::Offset3D{0, 0, 0};
            region.imageExtent 						= vk::Extent3D{width, height, 1};
            region.imageSubresource.aspectMask 		= vk::ImageAspectFlagBits::eColor;
            region.imageSubresource.mipLevel 		= 0;
            region.imageSubresource.baseArrayLayer 	= 0;
            region.imageSubresource.layerCount 		= 1;

            commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, 1, &region);

            endSingleTimeCommands(commandBuffer, graphicsQueue);
        }

    }
}