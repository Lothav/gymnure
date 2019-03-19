//
// Created by tracksale on 8/31/17.
//

#ifndef OBSIDIAN2D_BUFFERIMAGE_H
#define OBSIDIAN2D_BUFFERIMAGE_H

#include <ApplicationData.hpp>
#include "Memory/Memory.h"

struct ImageProps
{
    uint32_t              width{};
    uint32_t              height{};
    VkFormat              format = VK_FORMAT_UNDEFINED;
    VkImageTiling         tiling{};
    VkImageUsageFlags     usage{};
    VkImageAspectFlags    aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    VkComponentMapping    component{};
};

struct MemoryProps
{
    VkMemoryPropertyFlags props_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
};

namespace Engine
{
    namespace Memory
    {
        class BufferImage
        {

        private:

            MemoryProps mem_props_;
            ImageProps img_pros_;

        public:

            VkImage 		image   = nullptr;
            VkDeviceMemory 	mem     = nullptr;
            VkImageView 	view    = nullptr;
            VkFormat        format;

            BufferImage(struct MemoryProps memory_pro, struct ImageProps img_props, VkImage image_ptr = VK_NULL_HANDLE)
            {
                this->img_pros_  = img_props;
                this->mem_props_ = memory_pro;
                this->format     = img_props.format;

                image = image_ptr == nullptr ? createImage() : image_ptr;

                // Create Imageview
                VkImageViewCreateInfo viewInfo = {};
                viewInfo.sType 								= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                viewInfo.image 								= this->image;
                viewInfo.viewType 							= VK_IMAGE_VIEW_TYPE_2D;
                viewInfo.format 							= this->format;
                viewInfo.components                         = this->img_pros_.component;
                viewInfo.subresourceRange.aspectMask 		= this->img_pros_.aspectMask;
                viewInfo.subresourceRange.baseMipLevel 		= 0;
                viewInfo.subresourceRange.levelCount 		= 1;
                viewInfo.subresourceRange.baseArrayLayer 	= 0;
                viewInfo.subresourceRange.layerCount 		= 1;

                auto res = vkCreateImageView(ApplicationData::data->device, &viewInfo, nullptr, &this->view);
                assert(res == VK_SUCCESS);
            }

            explicit BufferImage(struct ImageProps img_props, VkImage image = VK_NULL_HANDLE) : BufferImage({}, img_props, image) {}

            ~BufferImage()
            {
                auto device = ApplicationData::data->device;

                if(image != nullptr) vkDestroyImage(device, image, nullptr);
                if(view  != nullptr) vkDestroyImageView(device, view, nullptr);
                if(mem   != nullptr) vkFreeMemory(device, mem, nullptr);
            }

            void* operator new(std::size_t size)
            {
                return mem::Provider::getMemory(size);
            }

            void operator delete(void* ptr)
            {
                // Do not free memory here!
            }

        private:

            VkImage createImage()
            {
                VkResult res;
                VkImage image;

                // Create Image
                VkImageCreateInfo imageInfo = {};
                imageInfo.sType 		= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
                imageInfo.imageType 	= VK_IMAGE_TYPE_2D;
                imageInfo.extent.width 	= img_pros_.width;
                imageInfo.extent.height = img_pros_.height;
                imageInfo.extent.depth 	= 1;
                imageInfo.mipLevels 	= 1;
                imageInfo.arrayLayers 	= 1;
                imageInfo.format 		= img_pros_.format;
                imageInfo.tiling 		= img_pros_.tiling;
                imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                imageInfo.usage 		= img_pros_.usage;
                imageInfo.samples 		= VK_SAMPLE_COUNT_1_BIT;
                imageInfo.sharingMode 	= VK_SHARING_MODE_EXCLUSIVE;

                auto device = ApplicationData::data->device;

                res = vkCreateImage(device, &imageInfo, nullptr, &image);
                assert(res == VK_SUCCESS);

                // Allocate Image Memory
                VkMemoryRequirements mem_reqs;
                vkGetImageMemoryRequirements(device, image, &mem_reqs);

                VkMemoryAllocateInfo mem_alloc = {};
                mem_alloc.sType			  = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                mem_alloc.pNext			  = nullptr;
                mem_alloc.allocationSize  = 0;
                mem_alloc.memoryTypeIndex = 0;
                mem_alloc.allocationSize  = mem_reqs.size;

                bool pass = Memory::findMemoryType(mem_reqs.memoryTypeBits, mem_props_.props_flags, &mem_alloc.memoryTypeIndex);
                assert(pass);

                res = vkAllocateMemory(device, &mem_alloc, nullptr, &mem);
                assert(res == VK_SUCCESS);

                // Bind Image to Memory
                res = vkBindImageMemory(device, image, mem, 0);
                assert(res == VK_SUCCESS);

                return image;
            }
        };
    }
}
#endif //OBSIDIAN2D_BUFFERIMAGE_H
