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
            vk::Format              format = vk::Format::eUndefined;
            vk::ImageTiling         tiling{};
            vk::ImageUsageFlags     usage{};
            vk::MemoryPropertyFlags image_props_flags;
            vk::ComponentMapping    component{};
        };

        class BufferImage
        {

        private:

            bool image_created = false;

        public:

            vk::Image image = {};
            vk::ImageView view = {};
            vk::DeviceMemory memory = {};

            /**
             * Create an Image, Memory and ImageView buffers.
             * */
            explicit BufferImage(const struct ImageProps& img_props)
                : image(createImage(img_props)), view(createImageView(img_props)),
                // 'image_created = true' marks that Image has been created by this class.
                // If it wont (came from another resource, e.g. swapchain) we cant destroy it here! (see destructor)
                image_created(true) {};

            /**
             * Create a image buffer using an external Image/Memory resources.
             * */
            explicit BufferImage(const struct ImageProps& img_props, vk::Image image_ptr)
                : image(image_ptr), view(createImageView(img_props)) {};

            ~BufferImage();

        private:

            vk::Image createImage(const struct ImageProps& img_props);
            vk::ImageView createImageView(const struct ImageProps& img_props) const;
        };
    }
}
#endif //OBSIDIAN2D_BUFFERIMAGE_H
