//
// Created by luiz0tavio on 8/30/18.
//

#ifndef VOLKANO_DEBUG_HPP
#define VOLKANO_DEBUG_HPP

#include "vulkan/vulkan.h"
#include <iostream>
#include <cassert>

namespace Engine
{
    class Debug
    {

    private:

        static VkDebugUtilsMessengerEXT dbg_messenger;

        static PFN_vkCreateDebugUtilsMessengerEXT CreateDebugUtilsMessengerEXT;
        static PFN_vkDestroyDebugUtilsMessengerEXT DestroyDebugUtilsMessengerEXT;

    public:

        Debug() = delete;

        static void init();
        static void destroy();

        static inline void logInfo(const std::string &log_msg)
        {
        #ifdef DEBUG
            std::cout << log_msg << std::endl;
        #endif
        };

        static inline void logError(const std::string &log_msg)
        {
        #ifdef DEBUG
            std::cerr << log_msg << std::endl;
        #endif
        };
    };
}

#endif //VOLKANO_DEBUG_HPP
