
#ifndef GYMNURE_APPLICATIONDATA_HPP
#define GYMNURE_APPLICATIONDATA_HPP

#include <zconf.h>
#include <vector>

#include <memancpp/Allocator.hpp>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan.hpp>

namespace Engine
{
    struct Data
    {
        uint32_t                                view_width;
        uint32_t                                view_height;

        vk::Instance 		                    instance;
        vk::SurfaceKHR                          surface;
        vk::Device 								device;
        vk::PhysicalDevice                      gpu;
        vk::CommandPool                         graphic_command_pool;

        u_int32_t							 	queue_family_count;
        vk::PhysicalDeviceMemoryProperties 		memory_properties;
        std::vector<vk::QueueFamilyProperties,
            mem::StdAllocator<
                vk::QueueFamilyProperties>>     queue_family_props;
    };

    class ApplicationData
    {
        public:
            static Data* data;
    };

}
#endif //GYMNURE_APPLICATIONDATA_HPP
