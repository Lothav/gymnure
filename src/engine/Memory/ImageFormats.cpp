
#include "ImageFormats.hpp"


namespace Engine
{
    namespace Memory
    {
        std::map<ImageType, vk::Format> ImageFormats::formats_ = {};
        vk::SurfaceFormatKHR ImageFormats::surface_format {};

        vk::Format ImageFormats::getImageFormat(ImageType type)
        {
            auto app_data = ApplicationData::data;

            switch (type)
            {
                case COLOR_TEXTURE:
                    if(formats_.find(COLOR_TEXTURE) == formats_.end()){
                        // @TODO: Check available formats
                        formats_[COLOR_TEXTURE] = vk::Format::eR16G16B16A16Sfloat;
                    }

                    return formats_[COLOR_TEXTURE];

                case DEPTH_STENCIL:
                    if(formats_.find(DEPTH_STENCIL) == formats_.end()){
                        // Since all depth formats may be optional, we need to find a suitable depth format to use
                        // Start with the highest precision packed format
                        std::vector<vk::Format> depthFormats = {
                            vk::Format::eD32SfloatS8Uint,
                            vk::Format::eD32Sfloat,
                            vk::Format::eD24UnormS8Uint,
                            vk::Format::eD16UnormS8Uint,
                            vk::Format::eD16Unorm,
                        };

                        auto d_format_it = std::find_if(depthFormats.begin(), depthFormats.end(), [app_data](vk::Format format) -> bool {
                            vk::FormatProperties formatProps = app_data->gpu.getFormatProperties(format);
                            return (formatProps.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment) != (vk::FormatFeatureFlagBits)0;
                        });

                        if(d_format_it != depthFormats.end())
                            formats_[DEPTH_STENCIL] = *d_format_it;
                        else
                            throw "Cannot find an optimal valid Depth format!";
                    }

                    return formats_[DEPTH_STENCIL];

                default:
                    throw "getImageFormat(): Invalid ImageType!";
            }
        }

        vk::SurfaceFormatKHR ImageFormats::getSurfaceFormat()
        {
            auto app_data = ApplicationData::data;

            if(surface_format == vk::SurfaceFormatKHR{})
            {
                uint32_t formatCount;

                vk::Result res = app_data->gpu.getSurfaceFormatsKHR(app_data->surface, &formatCount, nullptr, {});
                assert(res == vk::Result::eSuccess);

                std::vector<vk::SurfaceFormatKHR> surfFormats(formatCount);
                surfFormats = app_data->gpu.getSurfaceFormatsKHR(app_data->surface, {});
                // If the format list includes just one entry of vk::Format::eUndefined,
                // the surface has no preferred format.  Otherwise, at least one
                if (formatCount == 1 && surfFormats[0].format == vk::Format::eUndefined) {
                    surface_format.format = vk::Format::eB8G8R8A8Unorm;
                    surface_format.colorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
                } else {
                    // iterate over the list of available surface format and
                    // check for the presence of vk::Format::eB8G8R8A8Unorm
                    auto B8G8R8A8Unorm_format_it = std::find_if(surfFormats.begin(), surfFormats.end(),
                         [](vk::SurfaceFormatKHR surface_format) -> bool {
                            return surface_format.format == vk::Format::eB8G8R8A8Unorm;
                         });

                    if(B8G8R8A8Unorm_format_it != surfFormats.end()){
                        surface_format = *B8G8R8A8Unorm_format_it;
                    } else {
                        // in case vk::Format::eB8G8R8A8Unorm is not available
                        // select the first available color format
                        surface_format = surfFormats[0];
                    }
                }
            }

            return surface_format;
        }
    }
}