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

            bool image_created = false;

        public:

            vk::Image image = {};
            vk::ImageView view  = {};
            vk::DeviceMemory memory = {};

            /**
             * Create a image buffer using an external Image/Memory resources.
             * */
            BufferImage(const struct ImageViewProps& image_view_props, vk::Image image_ptr)
            {
                this->image = image_ptr;
                this->view = createImageView(image_view_props);
            };

            /**
             * Create an Image, Memory and ImageView buffers.
             * */
            BufferImage(const struct ImageViewProps& image_view_props, const struct ImageProps& img_props)
            {
                image = createImage(img_props, image_view_props.format);

                // Mark that Image has been created by this class.
                // If it wont (came from another resource, e.g. swapchain) we cant destroy it here! (see destructor)
                image_created = true;

                this->view = createImageView(image_view_props);
            };

            ~BufferImage();

        private:

            vk::Image createImage(const struct ImageProps& img_props, vk::Format format);
            vk::ImageView createImageView(const struct ImageViewProps& image_view_props) const;
        };
    }
}
#endif //OBSIDIAN2D_BUFFERIMAGE_H
