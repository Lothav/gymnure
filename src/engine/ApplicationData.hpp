
#ifndef GYMNURE_APPLICATIONDATA_HPP
#define GYMNURE_APPLICATIONDATA_HPP

#include <zconf.h>
#include <vector>

#include <memancpp/Allocator.hpp>
#include <vulkan/vulkan_core.h>

namespace Engine
{
    struct Data
    {
        uint32_t                                view_width;
        uint32_t                                view_height;

        VkInstance 		                        instance;
        VkSurfaceKHR                            surface;
        VkDevice 								device;
        VkPhysicalDevice                        gpu;
        VkRenderPass                            render_pass;
        VkCommandPool                           graphic_command_pool;

        u_int32_t							 	queue_family_count;
        u_int32_t                               queueGraphicFamilyIndex;
        u_int32_t                               queueComputeFamilyIndex;
        VkPhysicalDeviceMemoryProperties 		memory_properties;
        std::vector<VkQueueFamilyProperties,
            mem::StdAllocator<
                VkQueueFamilyProperties>>       queue_family_props;
    };

    class ApplicationData
    {
        public:
            static Data data;
    };

    Data ApplicationData::data = {};

}
#endif //GYMNURE_APPLICATIONDATA_HPP
