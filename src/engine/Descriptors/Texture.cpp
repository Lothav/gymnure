#include <ApplicationData.hpp>
#include <Util/Util.h>
#include <memory>
#include "Texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

namespace Engine
{
    namespace Descriptors
    {
        Texture::Texture(std::unique_ptr<Memory::BufferImage> buffer_image) : buffer_image_(std::move(buffer_image))
        {
            createSampler();
        }

        Texture::Texture(vk::Image image_ptr, uint32_t tex_width, uint32_t tex_height)
        {
            Memory::ImageProps img_props = {};
            img_props.width             = tex_width;
            img_props.height            = tex_height;
            img_props.format            = vk::Format::eR8G8B8A8Unorm;
            img_props.usage             = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled;
            img_props.tiling            = vk::ImageTiling::eOptimal;
            img_props.image_props_flags = vk::MemoryPropertyFlagBits::eDeviceLocal;

            buffer_image_ = std::make_unique<Memory::BufferImage>(img_props, image_ptr);

            if(!buffer_image_)
                Debug::logErrorAndDie("Fail to create Texture: unable to create TextureImage!");

            createSampler();
        }

        Texture::Texture(const std::string& texture_path)
        {
            int texWidth, texHeight, texChannels;

            // Load texture and write to vk::Image
            if(texture_path.empty())
                Debug::logErrorAndDie("Fail to create Texture: string path is empty!");

            auto assets_texture_path = std::string(ASSETS_FOLDER_PATH_STR) + "/" + texture_path;

            stbi_uc *pixels = stbi_load(assets_texture_path.data(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

            if(!pixels)
                throw "Cannot stbi_load pixels!";

            auto pixel_count = static_cast<size_t>(texWidth * texHeight * 4); // 4 channels

            struct BufferData stagingBufferData = {};
            stagingBufferData.usage      = vk::BufferUsageFlagBits::eTransferSrc;
            stagingBufferData.properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
            stagingBufferData.count      = pixel_count;

            auto staging_buffer = std::make_unique<Memory::Buffer<stbi_uc>>(stagingBufferData);
            staging_buffer->updateBuffer(pixels);

            stbi_image_free(pixels);

            Memory::ImageProps img_props = {};
            img_props.width             = static_cast<uint32_t>(texWidth);
            img_props.height            = static_cast<uint32_t>(texHeight);
            img_props.format            = vk::Format::eR8G8B8A8Unorm;
            img_props.usage             = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
            img_props.tiling            = vk::ImageTiling::eOptimal;
            img_props.image_props_flags = vk::MemoryPropertyFlagBits::eDeviceLocal;

            buffer_image_ = std::make_unique<Memory::BufferImage>(img_props);

            if(!buffer_image_)
                Debug::logErrorAndDie("Fail to create Texture: unable to create TextureImage!");

            vk::Queue queue = ApplicationData::data->transfer_queue;

            transitionImageLayout(vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, queue);
            copyBufferToImage(staging_buffer->getBuffer(), static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), queue);
            transitionImageLayout(vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, queue);

            createSampler();
        }

        void Texture::createSampler()
        {
            vk::SamplerCreateInfo sampler_ci = {};
            sampler_ci.maxAnisotropy 		= 1.0f;
            sampler_ci.magFilter 			= vk::Filter::eLinear;
            sampler_ci.minFilter 			= vk::Filter::eLinear;
            sampler_ci.mipmapMode 		    = vk::SamplerMipmapMode::eLinear;
            sampler_ci.addressModeU 		= vk::SamplerAddressMode::eRepeat;
            sampler_ci.addressModeV 		= vk::SamplerAddressMode::eRepeat;
            sampler_ci.addressModeW 		= vk::SamplerAddressMode::eRepeat;
            sampler_ci.mipLodBias 		    = 0.0f;
            sampler_ci.compareOp 			= vk::CompareOp::eNever;
            sampler_ci.minLod 			    = 0.0f;
            sampler_ci.maxLod 			    = 0.0f;
            sampler_ci.maxAnisotropy 		= 1.0;
            sampler_ci.anisotropyEnable 	= VK_FALSE;
            sampler_ci.borderColor 		    = vk::BorderColor::eFloatOpaqueWhite;
            sampler_ = ApplicationData::data->device.createSampler(sampler_ci);

            buffer_info_.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
            buffer_info_.imageView   = buffer_image_->view;
            buffer_info_.sampler 	 = sampler_;
        }

        Texture::~Texture()
        {
            vkDestroySampler(ApplicationData::data->device, sampler_, nullptr);
        }

        vk::CommandBuffer Texture::beginSingleTimeCommands()
        {
            auto app_data = ApplicationData::data;

            vk::CommandBufferAllocateInfo allocInfo = {};
            allocInfo.level 			 = vk::CommandBufferLevel::ePrimary;
            allocInfo.commandPool 		 = app_data->graphic_command_pool;
            allocInfo.commandBufferCount = 1;

            vk::CommandBuffer commandBuffer;
            app_data->device.allocateCommandBuffers(&allocInfo, &commandBuffer);

            vk::CommandBufferBeginInfo beginInfo = {};
            beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

            commandBuffer.begin(&beginInfo);

            return commandBuffer;
        }

        void Texture::endSingleTimeCommands(vk::CommandBuffer commandBuffer, vk::Queue graphicsQueue)
        {
            auto app_data = ApplicationData::data;

            commandBuffer.end();

            vk::SubmitInfo submitInfo = {};
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers 	  = &commandBuffer;

            graphicsQueue.submit({submitInfo}, {});
            graphicsQueue.waitIdle(); // @TODO use fence to sync

            app_data->device.freeCommandBuffers(app_data->graphic_command_pool, 1, &commandBuffer);
        }

        void Texture::transitionImageLayout(vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::Queue graphicsQueue)
        {
            vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

            vk::ImageMemoryBarrier barrier = {};
            barrier.oldLayout 							= oldLayout;
            barrier.newLayout 							= newLayout;
            barrier.srcQueueFamilyIndex 				= VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex 				= VK_QUEUE_FAMILY_IGNORED;
            barrier.image 								= buffer_image_->image;
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

        void Texture::copyBufferToImage(vk::Buffer buffer, uint32_t width, uint32_t height, vk::Queue graphicsQueue)
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

            commandBuffer.copyBufferToImage(buffer, buffer_image_->image, vk::ImageLayout::eTransferDstOptimal, 1, &region);

            endSingleTimeCommands(commandBuffer, graphicsQueue);
        }
    }
}