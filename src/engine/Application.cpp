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

        vk::ApplicationInfo app_info_ = {};
        app_info_.pNext 				= nullptr;
        app_info_.pApplicationName 		= APP_NAME;
        app_info_.applicationVersion 	= 1;
        app_info_.pEngineName 			= APP_NAME;
        app_info_.engineVersion 		= 1;
        app_info_.apiVersion 			= VK_API_VERSION_1_0;

        vk::InstanceCreateInfo inst_info_ = {};
        inst_info_.pNext 					= nullptr;
        inst_info_.pApplicationInfo 		= &app_info_;
        inst_info_.enabledLayerCount 		= (uint32_t) _layer_names.size();
        inst_info_.ppEnabledLayerNames 		= !_layer_names.empty() ? _layer_names.data() : nullptr;
        inst_info_.enabledExtensionCount 	= (uint32_t) instance_extension_names.size();
        inst_info_.ppEnabledExtensionNames 	= instance_extension_names.data();

        DEBUG_CALL(app_data->instance = vk::createInstance(inst_info_));
    }

    void Application::destroy()
    {
        auto app_data = ApplicationData::data;

        app_data->device.waitIdle();
        for(auto &program: programs)
            delete program;
        delete sync_primitives;
        delete render_pass;
        if(app_data->surface)
            app_data->instance.destroySurfaceKHR(app_data->surface, nullptr);
        delete command_buffer;
        app_data->device.destroyCommandPool(app_data->graphic_command_pool, nullptr);
        for(auto &texture_image_mem : Descriptors::Textures::textureImageMemory)
            app_data->device.freeMemory(texture_image_mem, nullptr);
        app_data->device.destroy();
        Debug::destroy();
        app_data->instance.destroy();
    }

    void Application::draw()
    {
        auto app_data = ApplicationData::data;

        vk::Result res;

        auto swapchain_c = render_pass->getSwapChain()->getSwapChainKHR();

        DEBUG_CALL(
            res = app_data->device.acquireNextImageKHR(
                swapchain_c,
                UINT64_MAX,
                sync_primitives->imageAcquiredSemaphore,
                nullptr,
                &current_buffer_));

        vk::PipelineStageFlags pipe_stage_flags = vk::PipelineStageFlagBits::eColorAttachmentOutput;

        //for(auto& program_obj: programs) program_obj->descriptor_set->getUniformBuffer()->updateMVP();

        vk::CommandBuffer current_command_buffer = command_buffer->getCommandBuffers()[current_buffer_];

        vk::Fence current_buffer_fence = sync_primitives->getFence(current_buffer_);
        do {
            // Fences are created already signaled, so, we can wait for it before queue submit.
            DEBUG_CALL(ApplicationData::data->device.waitForFences({current_buffer_fence}, VK_TRUE, UINT64_MAX));
        } while (res == vk::Result::eTimeout);
        ApplicationData::data->device.resetFences({current_buffer_fence});

        vk::SubmitInfo submit_info = {};
        submit_info.pNext                     = nullptr;
        submit_info.waitSemaphoreCount        = 1;
        submit_info.pWaitDstStageMask         = &pipe_stage_flags;
        submit_info.commandBufferCount        = 1;
        submit_info.pCommandBuffers           = &current_command_buffer;
        submit_info.signalSemaphoreCount      = 1;
        submit_info.pWaitSemaphores           = &sync_primitives->imageAcquiredSemaphore;
        submit_info.pSignalSemaphores         = &sync_primitives->renderSemaphore;

        render_pass->getSwapChain()->getGraphicQueue().submit({submit_info}, current_buffer_fence);

        vk::PresentInfoKHR present = {};
        present.pNext 				  = nullptr;
        present.swapchainCount 		  = 1;
        present.pSwapchains 		  = &swapchain_c;
        present.pImageIndices 		  = &current_buffer_;
        present.pWaitSemaphores 	  = nullptr;
        present.waitSemaphoreCount 	  = 0;
        present.pResults              = nullptr;

        if (sync_primitives->renderSemaphore) {
            present.pWaitSemaphores = &sync_primitives->renderSemaphore;
            present.waitSemaphoreCount = 1;
        }

        render_pass->getSwapChain()->getPresentQueue().presentKHR(present);
    }

    void Application::prepare()
    {
        command_buffer->bindGraphicCommandBuffer(programs, render_pass, ApplicationData::data->view_width, ApplicationData::data->view_height);
    }

    void Application::setupSurface(const uint32_t width, const uint32_t height)
    {
        auto app_data = ApplicationData::data;
        vk::Result res;

        std::vector<vk::PhysicalDevice> gpu_vector = {};
        res = app_data->instance.enumeratePhysicalDevices(&app_data->queue_family_count, nullptr);
        assert(res == vk::Result::eSuccess && app_data->queue_family_count);
        gpu_vector.resize(app_data->queue_family_count);
        app_data->instance.enumeratePhysicalDevices(&app_data->queue_family_count, gpu_vector.data());
        assert(res == vk::Result::eSuccess);

        std::string device_log = "========================================================\n";
        device_log += "Devices found:\n";
        for (uint i = 0; i < gpu_vector.size(); i++) {
            vk::PhysicalDeviceProperties device_properties = gpu_vector[i].getProperties();
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

        app_data->gpu.getQueueFamilyProperties(&app_data->queue_family_count, nullptr);
        assert(app_data->queue_family_count >= 1);
        app_data->queue_family_props.resize(app_data->queue_family_count);
        app_data->gpu.getQueueFamilyProperties(&app_data->queue_family_count, app_data->queue_family_props.data());

        app_data->gpu.getMemoryProperties(&app_data->memory_properties);

        auto queueGraphicFamilyIndex = UINT_MAX;
        auto queueComputeFamilyIndex = UINT_MAX;

        for (unsigned int i = 0; i < app_data->queue_family_count; i++) {
            if (app_data->queue_family_props[i].queueFlags & vk::QueueFlagBits::eGraphics) {
                queueGraphicFamilyIndex = i;
                // Generic queue that support compute.
                if (queueComputeFamilyIndex == UINT_MAX && app_data->queue_family_props[i].queueFlags & vk::QueueFlagBits::eCompute) {
                    queueComputeFamilyIndex = i;
                }
            } else {
                // Some GPU's have a dedicate compute queue. Try to find it.
                if (app_data->queue_family_props[i].queueFlags & vk::QueueFlagBits::eCompute) {
                    queueComputeFamilyIndex = i;
                }
            }
        }

        // Check if found both Queues.
        assert(queueComputeFamilyIndex != UINT_MAX && queueGraphicFamilyIndex != UINT_MAX);

        float queue_priorities[1] = {0.0};

        vk::DeviceQueueCreateInfo queue_info = {};
        queue_info.pNext 			= nullptr;
        queue_info.queueCount 		= 1;
        queue_info.pQueuePriorities = queue_priorities;
        queue_info.queueFamilyIndex = queueGraphicFamilyIndex;

        std::vector<const char *> device_extension_names;
        device_extension_names.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        vk::DeviceCreateInfo device_info = {};
        device_info.pNext 					= nullptr;
        device_info.queueCreateInfoCount 	= 1;
        device_info.pQueueCreateInfos 		= &queue_info;
        device_info.enabledExtensionCount 	= (uint32_t)device_extension_names.size();
        device_info.ppEnabledExtensionNames = device_info.enabledExtensionCount ? device_extension_names.data() : nullptr;
        device_info.enabledLayerCount 		= 0;
        device_info.ppEnabledLayerNames 	= nullptr;
        device_info.pEnabledFeatures 		= nullptr;

        res = app_data->gpu.createDevice(&device_info, nullptr, &app_data->device);
        assert(res == vk::Result::eSuccess);

        vk::CommandPoolCreateInfo cmd_pool_info = {};
        cmd_pool_info.pNext 			= nullptr;
        cmd_pool_info.queueFamilyIndex  = queueGraphicFamilyIndex;

        res = app_data->device.createCommandPool(&cmd_pool_info, nullptr, &app_data->graphic_command_pool);
        assert(res == vk::Result::eSuccess);

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
        sync_primitives = new SyncPrimitives::SyncPrimitives();
        sync_primitives->createSemaphore();
        sync_primitives->createFences(render_pass->getSwapChain()->getImageCount());

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
