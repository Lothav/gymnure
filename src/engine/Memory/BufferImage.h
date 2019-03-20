//
// Created by tracksale on 8/31/17.
//

#ifndef OBSIDIAN2D_BUFFERIMAGE_H
#define OBSIDIAN2D_BUFFERIMAGE_H

#include <ApplicationData.hpp>
#include "Memory/Memory.h"

struct ImageProps
{
    uint32_t                width{};
    uint32_t                height{};
    vk::Format              format = vk::Format::eUndefined;
    vk::ImageTiling         tiling{};
    vk::ImageUsageFlags     usage{};
    vk::ImageAspectFlags    aspectMask = vk::ImageAspectFlagBits::eColor;
    vk::ComponentMapping    component{};
};

struct MemoryProps
{
    vk::MemoryPropertyFlags props_flags = vk::MemoryPropertyFlagBits::eDeviceLocal;
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

            vk::Image 		    image   = nullptr;
            vk::DeviceMemory 	mem     = nullptr;
            vk::ImageView 	    view    = nullptr;
            vk::Format          format;

            BufferImage(const struct MemoryProps& memory_pro, const struct ImageProps& img_props, vk::Image image_ptr = {});

            explicit BufferImage(const struct ImageProps& img_props, vk::Image image = {}) : BufferImage({}, img_props, image) {}

            ~BufferImage();

            void* operator new(std::size_t size)
            {
                return mem::Provider::getMemory(size);
            }

            void operator delete(void* ptr)
            {
                // Do not free memory here!
            }

        private:

            vk::Image createImage();
        };
    }
}
#endif //OBSIDIAN2D_BUFFERIMAGE_H
