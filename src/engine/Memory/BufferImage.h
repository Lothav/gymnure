//
// Created by tracksale on 8/31/17.
//

#ifndef OBSIDIAN2D_BUFFERIMAGE_H
#define OBSIDIAN2D_BUFFERIMAGE_H

#include <ApplicationData.hpp>
#include "Memory/Memory.h"


namespace Engine
{
    namespace Memory
    {
        struct ImageProps
        {
            uint32_t                width = 0;
            uint32_t                height = 0;
            vk::ImageTiling         tiling{};
            vk::ImageUsageFlags     usage{};
            vk::MemoryPropertyFlags image_props_flags;
        };

        struct ImageViewProps
        {
            vk::Format              format = vk::Format::eUndefined;
            vk::ImageAspectFlags    aspectMask = vk::ImageAspectFlagBits::eColor;
            vk::ComponentMapping    component{};
        };

        class BufferImage
        {

        private:

            vk::DeviceMemory 	mem = nullptr;

        public:

            vk::Image 		    image = nullptr;
            vk::ImageView 	    view  = nullptr;

            BufferImage(const struct ImageViewProps& image_view_props, vk::Image image_ptr);

            BufferImage(const struct ImageViewProps& image_view_props, const struct ImageProps& img_props) :
                BufferImage(image_view_props, createImage(img_props, image_view_props.format)) {};

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

            vk::Image createImage(const struct ImageProps& img_props, vk::Format format);
        };
    }
}
#endif //OBSIDIAN2D_BUFFERIMAGE_H
