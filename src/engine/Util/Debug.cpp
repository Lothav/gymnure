//
// Created by luiz0tavio on 8/30/18.
//

#include <ApplicationData.hpp>
#include "Debug.hpp"

vk::DebugUtilsMessengerEXT Engine::Debug::dbg_messenger;

void Engine::Debug::init()
{
    auto instance = ApplicationData::data->instance;

    PFN_vkDebugUtilsMessengerCallbackEXT debug_messenger_callback = [](
            VkDebugUtilsMessageSeverityFlagBitsEXT       messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT              messageType,
            const VkDebugUtilsMessengerCallbackDataEXT*  pCallbackData,
            void*                                        pUserData) -> VkBool32
    {
        std::string message;

        if(messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)      message = "VERBOSE: ";
        else if(messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)    message = "INFO: ";
        else if(messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) message = "WARNING: ";
        else if(messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)   message = "ERROR: ";
        else message = "UNKNOWN: ";
        message += pCallbackData->pMessage;

        Debug::logInfo(message);

        return VK_FALSE;
    };

    // Create a Debug Utils Messenger that will trigger our callback for any warning or error.
    vk::DebugUtilsMessengerCreateInfoEXT dbg_messenger_create_info;
    dbg_messenger_create_info.pNext           = nullptr;
    dbg_messenger_create_info.pUserData       = nullptr;
    dbg_messenger_create_info.pfnUserCallback = debug_messenger_callback;

    dbg_messenger_create_info.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;

    dbg_messenger_create_info.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
        vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;

    auto pfnGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr) instance.getProcAddr("vkGetInstanceProcAddr");
    vk::DispatchLoaderDynamic dldi(instance, pfnGetInstanceProcAddr);
    dbg_messenger = instance.createDebugUtilsMessengerEXT(dbg_messenger_create_info, nullptr, dldi);
}

void Engine::Debug::destroy()
{
    auto instance = ApplicationData::data->instance;

    auto pfnGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr) instance.getProcAddr("vkGetInstanceProcAddr");
    vk::DispatchLoaderDynamic dldi(instance, pfnGetInstanceProcAddr);
    instance.destroyDebugUtilsMessengerEXT(dbg_messenger, nullptr, dldi);
}