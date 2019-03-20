//
// Created by tracksale on 8/31/17.
//
#include "BufferImage.h"

namespace Engine
{
    namespace Memory
    {
        BufferImage::BufferImage(const struct MemoryProps& memory_pro, const struct ImageProps& img_props, vk::Image image_ptr = {})
        {
            this->img_pros_  = img_props;
            this->mem_props_ = memory_pro;
            this->format     = img_props.format;

            image = image_ptr ? image_ptr : createImage();

            // Create Imageview
            vk::ImageViewCreateInfo viewInfo = {};
            viewInfo.image 								= this->image;
            viewInfo.viewType 							= vk::ImageViewType::e2D;
            viewInfo.format 							= this->format;
            viewInfo.components                         = this->img_pros_.component;
            viewInfo.subresourceRange.aspectMask 		= this->img_pros_.aspectMask;
            viewInfo.subresourceRange.baseMipLevel 		= 0;
            viewInfo.subresourceRange.levelCount 		= 1;
            viewInfo.subresourceRange.baseArrayLayer 	= 0;
            viewInfo.subresourceRange.layerCount 		= 1;

            vk::Result res = ApplicationData::data->device.createImageView(&viewInfo, nullptr, &this->view);
            assert(res == vk::Result::eSuccess);
        }

        BufferImage::BufferImage(const struct ImageProps& img_props, vk::Image image = {}) : BufferImage({}, img_props, image) {}

        BufferImage::~BufferImage()
        {
            auto device = ApplicationData::data->device;

            if(image) device.destroyImage(image, nullptr);
            if(view) device.destroyImageView(view, nullptr);
            if(mem) device.freeMemory(mem, nullptr);
        }


        vk::Image BufferImage::createImage()
        {
            vk::Result res;
            vk::Image image;

            // Create Image
            vk::ImageCreateInfo imageInfo = {};
            imageInfo.imageType 	= vk::ImageType::e2D;
            imageInfo.extent.width 	= img_pros_.width;
            imageInfo.extent.height = img_pros_.height;
            imageInfo.extent.depth 	= 1;
            imageInfo.mipLevels 	= 1;
            imageInfo.arrayLayers 	= 1;
            imageInfo.format 		= img_pros_.format;
            imageInfo.tiling 		= img_pros_.tiling;
            imageInfo.initialLayout = vk::ImageLayout::eUndefined;
            imageInfo.usage 		= img_pros_.usage;
            imageInfo.samples 		= vk::SampleCountFlagBits::e1;
            imageInfo.sharingMode 	= vk::SharingMode::eExclusive;

            auto device = ApplicationData::data->device;

            res = device.createImage(&imageInfo, nullptr, &image);
            assert(res == vk::Result::eSuccess);

            // Allocate Image Memory
            vk::MemoryRequirements mem_reqs{};
            device.getImageMemoryRequirements(image, &mem_reqs);

            vk::MemoryAllocateInfo mem_alloc = {};
            mem_alloc.pNext			  = nullptr;
            mem_alloc.allocationSize  = 0;
            mem_alloc.memoryTypeIndex = 0;
            mem_alloc.allocationSize  = mem_reqs.size;

            bool pass = Memory::findMemoryType(mem_reqs.memoryTypeBits, mem_props_.props_flags, &mem_alloc.memoryTypeIndex);
            assert(pass);

            res = device.allocateMemory(&mem_alloc, nullptr, &mem);
            assert(res == vk::Result::eSuccess);

            // Bind Image to Memory
            device.bindImageMemory(image, mem, 0);

            return image;
        }
    }
}
