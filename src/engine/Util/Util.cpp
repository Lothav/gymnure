#include "Util.h"

namespace Engine
{
    namespace Util
    {

        void Util::initViewports(VkCommandBuffer cmd_buffer, uint32_t width, uint32_t height)
        {
            VkViewport viewport;
            viewport.height 	= static_cast<float>(height);
            viewport.width 		= static_cast<float>(width);
            viewport.minDepth 	= 0.0f;
            viewport.maxDepth 	= 1.0f;
            viewport.x 			= 0;
            viewport.y 			= 0;

            vkCmdSetViewport(cmd_buffer, 0, 1, &viewport);
        }

        std::string Util::physicalDeviceTypeString(VkPhysicalDeviceType type)
        {
        #define CASE_STR(r) case VK_PHYSICAL_DEVICE_TYPE_##r: return #r
            switch (type)
            {
                CASE_STR(OTHER);
                CASE_STR(INTEGRATED_GPU);
                CASE_STR(DISCRETE_GPU);
                CASE_STR(VIRTUAL_GPU);
                default: return "UNKNOWN_DEVICE_TYPE";
            }
        #undef CASE_STR
        }

        void Util::initScissors(VkCommandBuffer cmd_buffer, uint32_t width, uint32_t height)
        {
            VkRect2D scissor;
            scissor.extent.width 	= (uint32_t)width;
            scissor.extent.height 	= (uint32_t)height;
            scissor.offset.x 		= 0;
            scissor.offset.y 		= 0;
            vkCmdSetScissor(cmd_buffer, 0, 1, &scissor);
        }

        VkShaderModule Util::loadSPIRVShader(const std::string& filename, VkDevice device)
        {
            long shaderSize;
            char* shaderCode = nullptr;

            std::string shader_file_path = std::string(ASSETS_FOLDER_PATH_STR) + "/shaders/" + filename;
            std::ifstream is(shader_file_path, std::ios::binary | std::ios::in | std::ios::ate);

            if (is.is_open())
            {
                shaderSize = is.tellg();
                assert(shaderSize > 0);

                is.seekg(0, std::ios::beg);
                // Copy file contents into a buffer
                shaderCode = new char[shaderSize];
                is.read(shaderCode, shaderSize);
                is.close();
            }
            if (shaderCode)
            {
                // Create a new shader module that will be used for pipeline creation
                VkShaderModuleCreateInfo moduleCreateInfo{};
                moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
                moduleCreateInfo.codeSize = static_cast<size_t>(shaderSize);
                moduleCreateInfo.pCode = (uint32_t*)shaderCode;

                VkShaderModule shaderModule;
                VkResult res = vkCreateShaderModule(device, &moduleCreateInfo, nullptr, &shaderModule);
                assert(res == VK_SUCCESS);

                delete[] shaderCode;

                return shaderModule;
            }
            else
            {
                std::cerr << "Error: Could not open shader file \"" << filename << "\"" << std::endl;
                return nullptr;
            }
        }
    }
}
