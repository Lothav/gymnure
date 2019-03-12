#include "Textures.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

namespace Engine
{
    namespace Descriptors
    {
        std::vector<VkDeviceMemory, mem::StdAllocator<VkDeviceMemory>> Textures::textureImageMemory;

        VkImage Textures::createTextureImage(
            VkPhysicalDevice physicalDevice,
            VkDevice device,
            const std::string &texture_path,
            VkCommandPool commandPool,
            VkQueue graphicQueue,
            VkPhysicalDeviceMemoryProperties memory_properties
        ) {
            VkImage textureImage;

            int texWidth, texHeight, texChannels;
            stbi_uc *pixels = stbi_load(texture_path.data(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
            auto imageSize = static_cast<VkDeviceSize>(texWidth * texHeight * 4);

            assert(pixels);

            struct BufferData stagingBufferData = {};

            stagingBufferData.device = device;
            stagingBufferData.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            stagingBufferData.physicalDevice = physicalDevice;
            stagingBufferData.properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            stagingBufferData.size = imageSize;

            auto *staging_buffer = new Memory::Buffer(stagingBufferData);

            void *data = nullptr;
            vkMapMemory(device, staging_buffer->mem, 0, imageSize, 0, &data);
            memcpy(data, pixels, static_cast<size_t>(imageSize));
            vkUnmapMemory(device, staging_buffer->mem);

            stbi_image_free(pixels);

            createImage(device, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight),
                        VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
                        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, memory_properties);

            transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED,
                                  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, commandPool, graphicQueue, device);
            copyBufferToImage(staging_buffer->buf, textureImage, static_cast<uint32_t>(texWidth),
                              static_cast<uint32_t>(texHeight), commandPool, graphicQueue, device);
            transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, commandPool, graphicQueue, device);

            delete staging_buffer;

            return textureImage;
        }
    }
}