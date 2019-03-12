#include <ApplicationData.hpp>
#include "Textures.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

namespace Engine
{
    namespace Descriptors
    {
        std::vector<VkDeviceMemory, mem::StdAllocator<VkDeviceMemory>> Textures::textureImageMemory;

        VkImage Textures::createTextureImage(const std::string &texture_path, VkQueue graphicQueue)
        {
            auto app_data = ApplicationData::data;

            VkImage textureImage;

            int texWidth, texHeight, texChannels;
            stbi_uc *pixels = stbi_load(texture_path.data(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
            auto imageSize = static_cast<VkDeviceSize>(texWidth * texHeight * 4);

            assert(pixels);

            struct BufferData stagingBufferData = {};

            stagingBufferData.usage          = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            stagingBufferData.properties     = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            stagingBufferData.size           = imageSize;

            auto *staging_buffer = new Memory::Buffer(stagingBufferData);

            void *data = nullptr;
            vkMapMemory(app_data->device, staging_buffer->mem, 0, imageSize, 0, &data);
            memcpy(data, pixels, static_cast<size_t>(imageSize));
            vkUnmapMemory(app_data->device, staging_buffer->mem);

            stbi_image_free(pixels);

            createImage(static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight),
                        VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
                        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage);

            transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED,
                                  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, graphicQueue);

            copyBufferToImage(staging_buffer->buf, textureImage, static_cast<uint32_t>(texWidth),
                              static_cast<uint32_t>(texHeight), graphicQueue);

            transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, graphicQueue);

            delete staging_buffer;

            return textureImage;
        }
    }
}