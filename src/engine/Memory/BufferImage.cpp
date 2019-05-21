#include "BufferImage.h"
#include "Util/Debug.hpp"

namespace Engine
{
    namespace Memory
    {
        BufferImage::~BufferImage()
        {
            auto device = ApplicationData::data->device;

            if(image_created) {
                if(image) device.destroyImage(image, nullptr);
                if(memory) device.freeMemory(memory, nullptr);
            }

            if(view) device.destroyImageView(view, nullptr);
        }

        vk::ImageView BufferImage::createImageView(const struct ImageProps& img_props) const
        {
            vk::ImageAspectFlags aspectMask = vk::ImageAspectFlagBits::eColor;
            if (img_props.usage & vk::ImageUsageFlagBits::eDepthStencilAttachment)
                aspectMask = vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;

            vk::ImageView image_view{};

            // Create Image View
            vk::ImageViewCreateInfo viewInfo = {};
            viewInfo.image 							 = this->image;
            viewInfo.viewType 						 = vk::ImageViewType::e2D;
            viewInfo.format 						 = img_props.format;
            viewInfo.components                      = img_props.component;
            viewInfo.subresourceRange.aspectMask 	 = aspectMask;
            viewInfo.subresourceRange.baseMipLevel 	 = 0;
            viewInfo.subresourceRange.levelCount 	 = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount 	 = 1;

            DEBUG_CALL(image_view = ApplicationData::data->device.createImageView(viewInfo));

            return image_view;
        }

        vk::Image BufferImage::createImage(const struct ImageProps& img_props)
        {
            auto app_data = ApplicationData::data;

            // Create Image
            vk::ImageCreateInfo imageInfo = {};
            imageInfo.imageType 	= vk::ImageType::e2D;
            imageInfo.extent.width 	= img_props.width;
            imageInfo.extent.height = img_props.height;
            imageInfo.extent.depth 	= 1;
            imageInfo.mipLevels 	= 1;
            imageInfo.arrayLayers 	= 1;
            imageInfo.format 		= img_props.format;
            imageInfo.tiling 		= img_props.tiling;
            imageInfo.initialLayout = vk::ImageLayout::eUndefined;
            imageInfo.usage 		= img_props.usage;
            imageInfo.samples 		= vk::SampleCountFlagBits::e1;
            imageInfo.sharingMode 	= vk::SharingMode::eExclusive;

            vk::Device device = app_data->device;

            vk::Image image = device.createImage(imageInfo);

            // Allocate Image Memory
            vk::MemoryRequirements mem_reqs{};
            DEBUG_CALL(mem_reqs = device.getImageMemoryRequirements(image));

            vk::MemoryAllocateInfo mem_alloc = {};
            mem_alloc.pNext			  = nullptr;
            mem_alloc.allocationSize  = 0;
            mem_alloc.memoryTypeIndex = 0;
            mem_alloc.allocationSize  = mem_reqs.size;
            mem_alloc.memoryTypeIndex = Memory::findMemoryType(mem_reqs.memoryTypeBits, img_props.image_props_flags);

            DEBUG_CALL(memory = device.allocateMemory(mem_alloc));

            // Bind Image to Memory
            device.bindImageMemory(image, memory, 0);

            return image;
        }
    }
}
