
#ifndef GYMNURE_DEFERRED_HPP
#define GYMNURE_DEFERRED_HPP

#include <memory>
#include "Memory/BufferImage.h"

namespace Engine
{
    namespace GraphicsPipeline
    {
        class Deferred
        {

        private:

            struct {
                std::unique_ptr<Memory::BufferImage> albedo;
            } g_buffer_;

        public:

            Deferred()
            {
                Memory::ImageProps img_props = {};
                img_props.width             = static_cast<uint32_t>(2048);
                img_props.height            = static_cast<uint32_t>(2048);
                img_props.usage             =                 // Image will be
                    vk::ImageUsageFlagBits::eSampled |        // sampled in shaders and
                    vk::ImageUsageFlagBits::eColorAttachment; // used as Framebuffer attachment
                img_props.tiling            = vk::ImageTiling::eOptimal;
                img_props.image_props_flags = vk::MemoryPropertyFlagBits::eDeviceLocal;

                g_buffer_.albedo = std::make_unique<Memory::BufferImage>(std::move(img_props));
            }
        };
    }
}
#endif //GYMNURE_DEFERRED_HPP
