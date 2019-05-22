#include "Application.hpp"

#include <Programs/Default.hpp>
#include <Util/Debug.hpp>
#include <Programs/Phong.hpp>
#include <Memory/ImageFormats.hpp>

namespace Engine
{
    uint32_t 						                        Application::current_buffer_ = 0;

    std::vector<std::shared_ptr<Programs::Program>>         Application::programs = {};
    std::unique_ptr<SyncPrimitives::SyncPrimitives>         Application::sync_primitives = nullptr;
    std::unique_ptr<CommandBuffers>                         Application::command_buffer = nullptr;
    std::vector<std::shared_ptr<RenderPass::FrameBuffer>> 	Application::frame_buffers = {};
    std::shared_ptr<RenderPass::SwapChain> 			        Application::swap_chain = nullptr;
    std::shared_ptr<Descriptors::Camera>                    Application::main_camera = nullptr;
    std::shared_ptr<RenderPass::RenderPass>                 Application::render_pass_ = nullptr;
    std::shared_ptr<Memory::BufferImage>                    Application::depth_buffer_ = nullptr;

    void Application::create(const std::vector<const char *>& instance_extension_names)
    {
        auto app_data = ApplicationData::data;
        
        std::vector<const char *> _layer_names = Util::Layers::getLayerNames();

        vk::ApplicationInfo app_info_ = {};
        app_info_.pNext 				    = nullptr;
        app_info_.pApplicationName 		    = APP_NAME;
        app_info_.applicationVersion 	    = 1;
        app_info_.pEngineName 			    = APP_NAME;
        app_info_.engineVersion 		    = 1;
        app_info_.apiVersion 			    = VK_MAKE_VERSION(1, 1, 101);

        vk::InstanceCreateInfo inst_info_ = {};
        inst_info_.pNext 					= nullptr;
        inst_info_.pApplicationInfo 		= &app_info_;
        inst_info_.enabledLayerCount 		= (uint32_t) _layer_names.size();
        inst_info_.ppEnabledLayerNames 		= !_layer_names.empty() ? _layer_names.data() : nullptr;
        inst_info_.enabledExtensionCount 	= (uint32_t) instance_extension_names.size();
        inst_info_.ppEnabledExtensionNames 	= instance_extension_names.data();

        DEBUG_CALL(app_data->instance = vk::createInstance(inst_info_));
    }

    std::shared_ptr<Descriptors::Camera> Application::getMainCamera()
    {
        return main_camera;
    }

    void Application::destroy()
    {
        auto app_data = ApplicationData::data;

        app_data->device.waitIdle();
        programs.clear();
        sync_primitives.reset();
        frame_buffers.clear();
        if(app_data->surface)
            app_data->instance.destroySurfaceKHR(app_data->surface, nullptr);
        command_buffer.reset();
        main_camera.reset();
        app_data->device.destroyCommandPool(app_data->graphic_command_pool, nullptr);
        app_data->device.destroy();
        Debug::destroy();
        app_data->instance.destroy();
    }

    void Application::draw()
    {
        vk::Result res = vk::Result::eNotReady;

        auto device = ApplicationData::data->device;
        auto swapchainKHR = swap_chain->getSwapChainKHR();

        DEBUG_CALL(
            std::tie(res, current_buffer_) = device.acquireNextImageKHR(
                swapchainKHR, UINT64_MAX, sync_primitives->imageAcquiredSemaphore, {}));
        assert(res == vk::Result::eSuccess);

        vk::PipelineStageFlags pipe_stage_flags = vk::PipelineStageFlagBits::eColorAttachmentOutput;

        vk::CommandBuffer current_command_buffer = command_buffer->getCommandBuffer(current_buffer_);

        vk::Fence current_buffer_fence = sync_primitives->getFence(current_buffer_);
        do {
            // Fences are created already signaled, so, we can wait for it before queue submit.
            res = device.waitForFences({current_buffer_fence}, VK_TRUE, UINT64_MAX);
        } while (res == vk::Result::eTimeout);
        DEBUG_CALL(device.resetFences({current_buffer_fence}));

        vk::SubmitInfo submit_info = {};
        submit_info.pNext                     = nullptr;
        submit_info.waitSemaphoreCount        = 1;
        submit_info.pWaitDstStageMask         = &pipe_stage_flags;
        submit_info.commandBufferCount        = 1;
        submit_info.pCommandBuffers           = &current_command_buffer;
        submit_info.signalSemaphoreCount      = 1;
        submit_info.pWaitSemaphores           = &sync_primitives->imageAcquiredSemaphore;
        submit_info.pSignalSemaphores         = &sync_primitives->renderSemaphore;

        DEBUG_CALL(swap_chain->getGraphicQueue().submit({submit_info}, current_buffer_fence));

        vk::PresentInfoKHR present = {};
        present.pNext 				  = nullptr;
        present.swapchainCount 		  = 1;
        present.pSwapchains 		  = &swapchainKHR;
        present.pImageIndices 		  = &current_buffer_;
        present.pWaitSemaphores 	  = nullptr;
        present.waitSemaphoreCount 	  = 0;
        present.pResults              = nullptr;

        if (sync_primitives->renderSemaphore) {
            present.pWaitSemaphores = &sync_primitives->renderSemaphore;
            present.waitSemaphoreCount = 1;
        }

        DEBUG_CALL(swap_chain->getGraphicQueue().presentKHR(&present));
    }

    void Application::prepare()
    {
        for (auto &program : programs)
            program->prepare(main_camera);

        command_buffer->bindGraphicCommandBuffer(swap_chain->getImageCount(), render_pass_, programs, frame_buffers);
    }

    void Application::setupSurface(const uint32_t& width, const uint32_t& height)
    {
        auto app_data = ApplicationData::data;

        std::vector<vk::PhysicalDevice> gpu_vector = {};
        app_data->instance.enumeratePhysicalDevices(&app_data->queue_family_count, nullptr, {});
        assert(app_data->queue_family_count > 0);
        gpu_vector.resize(app_data->queue_family_count);
        app_data->instance.enumeratePhysicalDevices(&app_data->queue_family_count, gpu_vector.data(), {});

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

        app_data->gpu.getQueueFamilyProperties(&app_data->queue_family_count, nullptr, {});
        assert(app_data->queue_family_count >= 1);
        app_data->queue_family_props.resize(app_data->queue_family_count);
        app_data->gpu.getQueueFamilyProperties(&app_data->queue_family_count, app_data->queue_family_props.data(), {});

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

        app_data->device = app_data->gpu.createDevice(device_info);

        vk::CommandPoolCreateInfo cmd_pool_info = {};
        cmd_pool_info.pNext 			= nullptr;
        cmd_pool_info.queueFamilyIndex  = queueGraphicFamilyIndex;

        app_data->graphic_command_pool = app_data->device.createCommandPool(cmd_pool_info);

        app_data->view_width  = width;
        app_data->view_height = height;

        // Init Swapchain
        swap_chain = std::make_shared<RenderPass::SwapChain>();

        // Create Depth Buffer
        {
            Memory::ImageProps img_props = {};
            img_props.width             = static_cast<uint32_t>(app_data->view_width);
            img_props.height            = static_cast<uint32_t>(app_data->view_height);
            img_props.format            = Memory::ImageFormats::getImageFormat(Memory::ImageType::DEPTH_STENCIL);
            img_props.usage             = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eDepthStencilAttachment;
            img_props.tiling            = vk::ImageTiling::eOptimal;
            img_props.image_props_flags = vk::MemoryPropertyFlagBits::eDeviceLocal;

            // Create Depth Buffer
            depth_buffer_ = std::make_unique<Memory::BufferImage>(img_props);
        }

        // Create Render Pass
        {
            std::vector<RenderPass::RpAttachments> rp_attachments = {};

            RenderPass::RpAttachments attch = {};
            attch.format        = Memory::ImageFormats::getSurfaceFormat().format;
            attch.clear         = true;
            attch.final_layout  = vk::ImageLayout::ePresentSrcKHR;
            attch.usage         = vk::ImageUsageFlagBits::eColorAttachment;
            rp_attachments.push_back(attch);

            attch.format        = Memory::ImageFormats::getImageFormat(Memory::ImageType::DEPTH_STENCIL);
            attch.clear         = true;
            attch.final_layout  = vk::ImageLayout::eDepthStencilAttachmentOptimal;
            attch.usage         = vk::ImageUsageFlagBits::eDepthStencilAttachment;
            rp_attachments.push_back(attch);

            render_pass_ = std::make_shared<RenderPass::RenderPass>(rp_attachments);
        }

        {
            std::vector<vk::ImageView> img_attachments = {};
            img_attachments.resize(2);

            for (int j = 0; j < swap_chain->getImageCount(); ++j)
            {
                img_attachments[0] = swap_chain->getSwapChainImageView(j);
                img_attachments[1] = depth_buffer_->view;

                frame_buffers.push_back(std::make_shared<RenderPass::FrameBuffer>(img_attachments, render_pass_));
            }
        }

        // Init Sync Primitives
        sync_primitives = std::make_unique<SyncPrimitives::SyncPrimitives>();
        sync_primitives->createSemaphore();
        sync_primitives->createFences(swap_chain->getImageCount());

        // Init Command Buffers
        command_buffer = std::make_unique<CommandBuffers>(swap_chain->getImageCount());

        // Init Main Camera
        main_camera = std::make_shared<Descriptors::Camera>(app_data->view_width, app_data->view_height);
    }

    void Application::addObjData(uint program_id, GymnureObjData&& data)
    {
        if(program_id >= programs.size()) {
            Debug::logInfo("Invalid program id! Object discarded!");
            return;
        }

        programs[program_id]->addObjData(std::move(data));
    }

    uint Application::createDefaultProgram()
    {
        auto program = std::make_unique<Programs::Default>(render_pass_->getRenderPass());
        programs.push_back(std::move(program));

        return static_cast<uint>(programs.size() - 1);
    }

    uint Application::createPhongProgram()
    {
        auto program = std::make_unique<Programs::Phong>(render_pass_->getRenderPass());
        programs.push_back(std::move(program));

        return static_cast<uint>(programs.size() - 1);
    }

}
