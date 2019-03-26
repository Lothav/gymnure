//
// Created by luizorv on 8/25/17.
//

#ifndef OBSIDIAN2D_TEXTURES_H
#define OBSIDIAN2D_TEXTURES_H
#include "vulkan/vulkan.h"

#include <vector>
#include <memancpp/Allocator.hpp>
#include <Memory/BufferImage.h>
#include "Memory/Memory.h"
#include "Memory/Buffer.h"

namespace Engine
{
	namespace Descriptors
	{
		class Texture
		{

		private:

			std::unique_ptr<Memory::BufferImage> buffer_image_;
			vk::Sampler sampler_ = {};

		public:

			Texture(const std::string &texture_path, vk::Queue graphicQueue);

			~Texture()
			{
				vkDestroySampler(ApplicationData::data->device, sampler_, nullptr);
			}

			void* operator new(std::size_t size)
			{
				return mem::Provider::getMemory(size);
			}

			void operator delete(void* ptr)
			{
				// Do not free memory here!
			}

			vk::ImageView getImageView() const;

			vk::Sampler getSampler() const;

		private:

			void transitionImageLayout(vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::Queue graphicsQueue);
			void copyBufferToImage(vk::Buffer buffer, uint32_t width, uint32_t height, vk::Queue graphicsQueue);

			vk::CommandBuffer beginSingleTimeCommands();
			void endSingleTimeCommands(vk::CommandBuffer commandBuffer, vk::Queue graphicsQueue);
		};
	}
}

#endif //OBSIDIAN2D_TEXTURES_H
