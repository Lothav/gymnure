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

			vk::ImageView getImageView() const
			{
				return buffer_image_->view;
			}

            vk::Image getImage() const
            {
                return buffer_image_->image;
			}

			vk::WriteDescriptorSet getWrite(vk::DescriptorSet dst_set, uint32_t dst_binding) const
			{
				vk::WriteDescriptorSet write = {};
				write.dstArrayElement = 0;
				write.descriptorCount = 1;
				write.descriptorType  = vk::DescriptorType::eCombinedImageSampler;
				write.dstBinding 	  = dst_binding;
				write.pImageInfo 	  = &buffer_info_;
				write.dstSet 		  = dst_set;

				return write;
			}

		private:

			void createSampler();
			void transitionImageLayout(vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::Queue graphicsQueue);
			void copyBufferToImage(vk::Buffer buffer, uint32_t width, uint32_t height, vk::Queue graphicsQueue);
            void submitPixels(unsigned char* pixels, uint32_t tex_width, uint32_t tex_height);

			vk::CommandBuffer beginSingleTimeCommands();
			void endSingleTimeCommands(vk::CommandBuffer commandBuffer, vk::Queue graphicsQueue);
		};
	}
}

#endif //OBSIDIAN2D_TEXTURES_H
