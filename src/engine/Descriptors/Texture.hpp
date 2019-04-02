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

			Texture(const std::string &texture_path);

			~Texture();

			void* operator new(std::size_t size)
			{
				return mem::Provider::getMemory(size);
			}

			void operator delete(void* ptr)
			{
				// Do not free memory here!
			}

			vk::WriteDescriptorSet getWrite(vk::DescriptorSet dst_set, uint32_t dst_binding) const
			{
				vk::DescriptorImageInfo *texture_info = new vk::DescriptorImageInfo();
				texture_info->imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
				texture_info->imageView   = buffer_image_->view;
				texture_info->sampler 	  = sampler_;

				vk::WriteDescriptorSet write = {};
				write.dstArrayElement = 0;
				write.descriptorCount = 1;
				write.descriptorType  = vk::DescriptorType::eCombinedImageSampler;
				write.dstBinding 	  = dst_binding;
				write.pImageInfo 	  = texture_info;
				write.dstSet 		  = dst_set;

				return write;
			}

		private:

			void transitionImageLayout(vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::Queue graphicsQueue);
			void copyBufferToImage(vk::Buffer buffer, uint32_t width, uint32_t height, vk::Queue graphicsQueue);

			vk::CommandBuffer beginSingleTimeCommands();
			void endSingleTimeCommands(vk::CommandBuffer commandBuffer, vk::Queue graphicsQueue);
		};
	}
}

#endif //OBSIDIAN2D_TEXTURES_H
