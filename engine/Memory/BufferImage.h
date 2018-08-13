//
// Created by tracksale on 8/31/17.
//

#ifndef OBSIDIAN2D_BUFFERIMAGE_H
#define OBSIDIAN2D_BUFFERIMAGE_H

#include "Memory/Memory.h"

struct ImageProps {
    uint32_t              width;
    uint32_t              height;
    VkFormat              format = VK_FORMAT_UNDEFINED;
    VkImageTiling         tiling;
    VkImageUsageFlags     usage;
    VkImageAspectFlags    aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
};

struct MemoryProps {
    VkMemoryPropertyFlags props_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    VkPhysicalDeviceMemoryProperties memory_props;
    VkDevice device;
};

namespace Engine
{
    namespace Memory
    {
        class BufferImage
        {

        private:
            MemoryProps _mem_props;
            ImageProps _img_pros;

        public:

            VkImage 		image   = nullptr;
            VkDeviceMemory 	mem     = nullptr;
            VkImageView 	view    = nullptr;
            VkFormat        format;

            ~BufferImage()
            {
                if(image != nullptr) vkDestroyImage(_mem_props.device, image, nullptr);
                if(view  != nullptr) vkDestroyImageView(_mem_props.device, view, nullptr);
                if(mem   != nullptr) vkFreeMemory(_mem_props.device, mem, nullptr);
            }

            BufferImage (struct MemoryProps memory_pro, struct ImageProps img_props, VkImage* images = nullptr)
            {
                this->_img_pros     = img_props;
                this->_mem_props    = memory_pro;
                this->format        = img_props.format;

                if(images == nullptr) {
                    createImage();
                    allocateMemory();
                } else {
                    image = *images;
                }

                createImageView();
            }

        private:

            void createImageView ()
            {
                VkResult res;

                VkImageViewCreateInfo viewInfo = {};
                viewInfo.sType 								= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                viewInfo.image 								= this->image;
                viewInfo.viewType 							= VK_IMAGE_VIEW_TYPE_2D;
                viewInfo.format 							= this->format;
                viewInfo.subresourceRange.aspectMask 		= this->_img_pros.aspectMask;
                viewInfo.subresourceRange.baseMipLevel 		= 0;
                viewInfo.subresourceRange.levelCount 		= 1;
                viewInfo.subresourceRange.baseArrayLayer 	= 0;
                viewInfo.subresourceRange.layerCount 		= 1;

                res = vkCreateImageView(this->_mem_props.device, &viewInfo, nullptr, &this->view);
                assert(res == VK_SUCCESS);
            }

            void allocateMemory ()
            {
                VkResult res;
                VkMemoryRequirements mem_reqs;

                vkGetImageMemoryRequirements(this->_mem_props.device, this->image, &mem_reqs);

                VkMemoryAllocateInfo mem_alloc = {};
                mem_alloc.sType				= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                mem_alloc.pNext				= nullptr;
                mem_alloc.allocationSize 	= 0;
                mem_alloc.memoryTypeIndex 	= 0;

                mem_alloc.allocationSize = mem_reqs.size;

                bool pass = Memory::findMemoryType(
                        _mem_props.memory_props,
                        mem_reqs.memoryTypeBits,
                        _mem_props.props_flags,
                        &mem_alloc.memoryTypeIndex
                );
                assert(pass);

                res = vkAllocateMemory(_mem_props.device, &mem_alloc, nullptr, &mem);
                assert(res == VK_SUCCESS);
                res = vkBindImageMemory(_mem_props.device, image, mem, 0);
                assert(res == VK_SUCCESS);
            }

            void createImage()
            {
                VkResult res;

                VkImageCreateInfo imageInfo = {};
                imageInfo.sType 					 = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
                imageInfo.imageType 				 = VK_IMAGE_TYPE_2D;
                imageInfo.extent.width 				 = _img_pros.width;
                imageInfo.extent.height 			 = _img_pros.height;
                imageInfo.extent.depth 				 = 1;
                imageInfo.mipLevels 				 = 1;
                imageInfo.arrayLayers 				 = 1;
                imageInfo.format 					 = _img_pros.format;
                imageInfo.tiling 					 = _img_pros.tiling;
                imageInfo.initialLayout 			 = VK_IMAGE_LAYOUT_UNDEFINED;
                imageInfo.usage 					 = _img_pros.usage;
                imageInfo.samples 					 = VK_SAMPLE_COUNT_1_BIT;
                imageInfo.sharingMode 				 = VK_SHARING_MODE_EXCLUSIVE;

                res = vkCreateImage(_mem_props.device, &imageInfo, nullptr, &image);
                assert(res == VK_SUCCESS);
            }
        };
    }
}
#endif //OBSIDIAN2D_BUFFERIMAGE_H
