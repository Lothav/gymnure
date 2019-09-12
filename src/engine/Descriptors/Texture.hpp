//
// Created by luizorv on 8/25/17.
//

#ifndef OBSIDIAN2D_TEXTURES_H
#define OBSIDIAN2D_TEXTURES_H
#include "vulkan/vulkan.h"

#include <vector>
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
			vk::DescriptorImageInfo buffer_info_{};

		public:

			Texture(vk::Image image_ptr, uint32_t tex_width, uint32_t tex_height);
			explicit Texture(const std::string &texture_path);
			explicit Texture(std::unique_ptr<Memory::BufferImage> buffer_image_);
            explicit Texture(unsigned char* pixels, uint32_t tex_width, uint32_t tex_height);

			~Texture();

			vk::ImageView getImageView() const;
            vk::Image getImage() const;
			vk::WriteDescriptorSet getWrite(vk::DescriptorSet dst_set, uint32_t dst_binding) const;

		private:

			void createSampler();
			void transitionImageLayout(vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::Queue graphicsQueue);
			void copyBufferToImage(vk::Buffer buffer, uint32_t width, uint32_t height, vk::Queue graphicsQueue);
            void submitPixels(unsigned char* pixels, uint32_t tex_width, uint32_t tex_height);

			static vk::CommandBuffer beginSingleTimeCommands();
			static void endSingleTimeCommands(vk::CommandBuffer commandBuffer, vk::Queue graphicsQueue);
		};
	}
}

#endif //OBSIDIAN2D_TEXTURES_H
