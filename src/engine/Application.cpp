#include "Application.hpp"

#include <Programs/Phong.h>
#include <Programs/Skybox.h>
#include <Programs/Default.hpp>
#include <Util/Debug.hpp>

namespace Engine
{
    std::vector<Programs::Program*> Application::programs = {};
    SyncPrimitives::SyncPrimitives* Application::sync_primitives = nullptr;
    CommandBuffers*                 Application::command_buffer = nullptr;
    uint32_t 						Application::current_buffer_ = 0;
    RenderPass::RenderPass* 		Application::render_pass = nullptr;

    void Application::create(const std::vector<const char *>& instance_extension_names)
    {
        auto app_data = ApplicationData::data;
        
        std::vector<const char *> _layer_names = Util::Layers::getLayerNames();

        VkApplicationInfo _app_info = {};
        _app_info.sType 				= VK_STRUCTURE_TYPE_APPLICATION_INFO;
        _app_info.pNext 				= nullptr;
        _app_info.pApplicationName 		= APP_NAME;
        _app_info.applicationVersion 	= 1;
        _app_info.pEngineName 			= APP_NAME;
        _app_info.engineVersion 		= 1;
        _app_info.apiVersion 			= VK_API_VERSION_1_1;

        VkInstanceCreateInfo _inst_info = {};
        memset(&_inst_info, 0, sizeof(VkInstanceCreateInfo));
        _inst_info.sType 					= VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        _inst_info.pNext 					= nullptr;
        _inst_info.flags 					= 0;
        _inst_info.pApplicationInfo 		= &_app_info;
        _inst_info.enabledLayerCount 		= (uint32_t) _layer_names.size();
        _inst_info.ppEnabledLayerNames 		= _layer_names.size() ? _layer_names.data() : nullptr;
        _inst_info.enabledExtensionCount 	= (uint32_t) instance_extension_names.size();
        _inst_info.ppEnabledExtensionNames 	= instance_extension_names.data();

        VkResult res = vkCreateInstance(&_inst_info, nullptr, &app_data->instance);
        assert(res == VK_SUCCESS);

        std::vector<VkPhysicalDevice> gpu_vector = {};
        res = vkEnumeratePhysicalDevices(app_data->instance, &app_data->queue_family_count, nullptr);
        assert(res == VK_SUCCESS && app_data->queue_family_count);
        gpu_vector.resize(app_data->queue_family_count);
        res = vkEnumeratePhysicalDevices(app_data->instance, &app_data->queue_family_count, gpu_vector.data());
        assert(res == VK_SUCCESS);

        std::string device_log = "========================================================\n";
        device_log += "Devices found:\n";
        for (uint i = 0; i < gpu_vector.size(); i++) {
            VkPhysicalDeviceProperties device_properties;
            vkGetPhysicalDeviceProperties(gpu_vector[i], &device_properties);
            device_log += "\tDevice[" + std::to_string(i) + "]: " + device_properties.deviceName + "\n";
            device_log += "\t\tType: " + Util::Util::physicalDeviceTypeString(device_properties.deviceType) + "\n";
            device_log += "\t\tAPI: " +
                std::to_string(device_properties.apiVersion >> 22) + "." +
                std::to_string((device_properties.apiVersion >> 12) & 0x3ff) + "." +
                std::to_string(device_properties.apiVersion & 0xfff) + "\n";
        }
        uint gpu_index = 0;
        device_log += "Using Device[" + std::to_string(gpu_index) + "]\n";
        device_log += "========================================================";

        Debug::logInfo(device_log);

        app_data->gpu = gpu_vector[gpu_index];

        vkGetPhysicalDeviceQueueFamilyProperties(app_data->gpu, &app_data->queue_family_count, nullptr);
        assert(app_data->queue_family_count >= 1);

        app_data->queue_family_props.resize(app_data->queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(app_data->gpu, &app_data->queue_family_count, app_data->queue_family_props.data());
        assert(app_data->queue_family_count >= 1);

        vkGetPhysicalDeviceMemoryProperties(app_data->gpu, &app_data->memory_properties);

        auto queueGraphicFamilyIndex = UINT_MAX;
        auto queueComputeFamilyIndex = UINT_MAX;

        bool foundGraphic = false;
        bool foundCompute = false;
        for (unsigned int i = 0; i < app_data->queue_family_count; i++) {
            if (app_data->queue_family_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                queueGraphicFamilyIndex = i;
                foundGraphic = true;
            }

            // Some GPU's have a dedicate compute queue. Try to find it.
            if ((app_data->queue_family_props[i].queueFlags & VK_QUEUE_COMPUTE_BIT) && ((app_data->queue_family_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)) {
                queueComputeFamilyIndex = i;
                foundCompute = true;
            }
        }

        // If no able to find a compute queue dedicated one, find a generic that support compute.
        if (!foundCompute) {
            for (unsigned int i = 0; i < app_data->queue_family_count; i++) {
                if (app_data->queue_family_props[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
                    queueComputeFamilyIndex = i;
                    foundCompute = true;
                }
            }
        }

        assert(foundGraphic && foundCompute && queueComputeFamilyIndex != UINT_MAX && queueGraphicFamilyIndex != UINT_MAX);

        float queue_priorities[1] = {0.0};

        VkDeviceQueueCreateInfo queue_info = {};
        queue_info.sType 			= VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_info.pNext 			= nullptr;
        queue_info.queueCount 		= 1;
        queue_info.pQueuePriorities = queue_priorities;
        queue_info.queueFamilyIndex = queueGraphicFamilyIndex;

        std::vector<const char *> device_extension_names;
        device_extension_names.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        VkDeviceCreateInfo device_info = {};
        device_info.sType 					= VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_info.pNext 					= nullptr;
        device_info.queueCreateInfoCount 	= 1;
        device_info.pQueueCreateInfos 		= &queue_info;
        device_info.enabledExtensionCount 	= (uint32_t)device_extension_names.size();
        device_info.ppEnabledExtensionNames = device_info.enabledExtensionCount ? device_extension_names.data() : nullptr;
        device_info.enabledLayerCount 		= 0;
        device_info.ppEnabledLayerNames 	= nullptr;
        device_info.pEnabledFeatures 		= nullptr;

        res = vkCreateDevice(app_data->gpu, &device_info, nullptr, &app_data->device);
        assert(res == VK_SUCCESS);

        VkCommandPoolCreateInfo cmd_pool_info = {};
        cmd_pool_info.sType 			= VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        cmd_pool_info.pNext 			= nullptr;
        cmd_pool_info.queueFamilyIndex  = queueGraphicFamilyIndex;
        cmd_pool_info.flags 			= 0;

        res = vkCreateCommandPool(app_data->device, &cmd_pool_info, nullptr, &app_data->graphic_command_pool);
        assert(res == VK_SUCCESS);
    }

    void Application::destroy()
    {
        auto app_data = ApplicationData::data;

        vkDeviceWaitIdle(app_data->device);
        for(auto &program: programs) delete program;
        delete sync_primitives;
        delete render_pass;
        if(app_data->surface != VK_NULL_HANDLE)
            vkDestroySurfaceKHR(app_data->instance, app_data->surface, nullptr);
        delete command_buffer;
        vkDestroyCommandPool(app_data->device, app_data->graphic_command_pool, nullptr);
        for(auto &i : Descriptors::Textures::textureImageMemory) vkFreeMemory(app_data->device, i, nullptr);
        vkDestroyDevice(app_data->device, nullptr);
        Debug::destroy();
        vkDestroyInstance(app_data->instance, nullptr);
    }

    void Application::draw()
    {
        VkResult res;
        VkSwapchainKHR swap_c = render_pass->getSwapChain()->getSwapChainKHR();

        res = vkAcquireNextImageKHR(ApplicationData::data->device, swap_c, UINT64_MAX, sync_primitives->imageAcquiredSemaphore, nullptr, &current_buffer_);
        assert(res == VK_SUCCESS);

        VkPipelineStageFlags pipe_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        for(auto& program_obj: programs) program_obj->descriptor_set->getUniformBuffer()->updateMVP();

        auto current_command_buffer = command_buffer->getCommandBuffers()[current_buffer_];

        VkSubmitInfo submit_info = {};
        submit_info.pNext                     = nullptr;
        submit_info.sType                     = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.waitSemaphoreCount        = 1;
        submit_info.pWaitDstStageMask         = &pipe_stage_flags;
        submit_info.commandBufferCount        = 1;
        submit_info.pCommandBuffers           = &current_command_buffer;
        submit_info.signalSemaphoreCount      = 1;
        submit_info.pWaitSemaphores           = &sync_primitives->imageAcquiredSemaphore;
        submit_info.pSignalSemaphores         = &sync_primitives->renderSemaphore;

        auto* current_buffer_fence = sync_primitives->getFence(current_buffer_);
        do {
            // Fences are created already signaled, so, we can wait for it before queue submit.
            res = vkWaitForFences(ApplicationData::data->device, 1, current_buffer_fence, VK_TRUE, UINT64_MAX);
        } while (res == VK_TIMEOUT);
        assert(res == VK_SUCCESS);
        vkResetFences(ApplicationData::data->device, 1, current_buffer_fence);

        res = vkQueueSubmit(render_pass->getSwapChain()->getGraphicQueue(), 1, &submit_info, *current_buffer_fence);
        assert(res == VK_SUCCESS);

        VkPresentInfoKHR present = {};
        present.sType 				  = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present.pNext 				  = nullptr;
        present.swapchainCount 		  = 1;
        present.pSwapchains 		  = &swap_c;
        present.pImageIndices 		  = &current_buffer_;
        present.pWaitSemaphores 	  = nullptr;
        present.waitSemaphoreCount 	  = 0;
        present.pResults              = nullptr;

        if (sync_primitives->renderSemaphore != VK_NULL_HANDLE) {
            present.pWaitSemaphores = &sync_primitives->renderSemaphore;
            present.waitSemaphoreCount = 1;
        }

        res = vkQueuePresentKHR(render_pass->getSwapChain()->getPresentQueue(), &present);
        assert(res == VK_SUCCESS);
    }

    void Application::prepare()
    {
        command_buffer->bindGraphicCommandBuffer(programs, render_pass, ApplicationData::data->view_width, ApplicationData::data->view_height);
    }

    void Application::setupSurface(const uint32_t width, const uint32_t height)
    {
        auto app_data = ApplicationData::data;

        app_data->view_width  = width;
        app_data->view_height = height;

        // Init Render Pass
        render_pass = new RenderPass::RenderPass();

        std::vector<struct rpAttachments> rp_attachments = {};

        struct rpAttachments attch = {};

        attch.format = render_pass->getSwapChain()->getSwapChainFormat();
        attch.clear  = true;
        rp_attachments.push_back(attch);

        attch.format = render_pass->getDepthBufferFormat();
        attch.clear  = true;
        rp_attachments.push_back(attch);

        render_pass->create(rp_attachments);

        // Init Sync Primitives
        sync_primitives = new SyncPrimitives::SyncPrimitives(app_data->device);
        sync_primitives->createSemaphore();
        sync_primitives->createFence(render_pass->getSwapChain()->getImageCount());

        // Init Command Buffers
        command_buffer = new CommandBuffers(render_pass->getSwapChain()->getImageCount());
    }

    void Application::addObjData(uint program_id, const GymnureObjData& data)
    {
        if(program_id >= programs.size()) assert(false);

        programs[program_id]->addObjData(data);
    }

    uint Application::createPhongProgram()
    {
        auto program = new Programs::Phong(render_pass->getSwapChain()->getGraphicQueue());
        program->init(render_pass->getRenderPass());

        programs.push_back(program);
        return static_cast<uint>(programs.size() - 1);
    }

    uint Application::createSkyboxProgram()
    {
        auto program = new Programs::Skybox(render_pass->getSwapChain()->getGraphicQueue());
        program->init(render_pass->getRenderPass());

        programs.push_back(program);
        return static_cast<uint>(programs.size() - 1);
    }

    uint Application::createDefaultProgram()
    {
        auto program = new Programs::Default(render_pass->getSwapChain()->getGraphicQueue());
        program->init(render_pass->getRenderPass());

        programs.push_back(program);
        return static_cast<uint>(programs.size() - 1);
    }

}
