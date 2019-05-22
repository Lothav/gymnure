#ifndef GYMNURE_FORMATS_HPP
#define GYMNURE_FORMATS_HPP

#include <map>
#include <vulkan/vulkan.hpp>
#include <ApplicationData.hpp>

namespace Engine
{
    namespace Memory
    {
        enum ImageType
        {
            COLOR_TEXTURE,
            DEPTH_STENCIL,
        };

        class ImageFormats
        {

        private:

            static std::map<ImageType, vk::Format> formats_;
            static vk::SurfaceFormatKHR surface_format;

        public:

            ImageFormats() = delete;

            static vk::Format getImageFormat(ImageType type);
            static vk::SurfaceFormatKHR getSurfaceFormat();

        };
    }
}

#endif //GYMNURE_FORMATS_HPP
