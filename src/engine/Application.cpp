#include "Application.hpp"

#include <Programs/Default.hpp>
#include <Util/Debug.hpp>
#include <Programs/Phong.hpp>
#include <Memory/ImageFormats.hpp>

namespace Engine
{
    std::vector<std::shared_ptr<Programs::Program>>         Application::programs = {};
    std::shared_ptr<Descriptors::Camera>                    Application::main_camera = nullptr;

    std::unique_ptr<GraphicsPipeline::Forward>              Application::forward = nullptr;

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
        forward.reset();
        if(app_data->surface)
            app_data->instance.destroySurfaceKHR(app_data->surface, nullptr);
        main_camera.reset();
        app_data->device.destroyCommandPool(app_data->graphic_command_pool, nullptr);
        app_data->device.destroy();
        Debug::destroy();
        app_data->instance.destroy();
    }

    void Application::draw()
    {
        // Draw pipelines
        forward->render();
    }

    void Application::prepare()
    {
        for (auto &program : programs)
            program->prepare(main_camera);

        // Prepare pipelines
        forward->prepare(programs);
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

        // Init Pipelines
        forward = std::make_unique<GraphicsPipeline::Forward>();

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
        auto program = std::make_unique<Programs::Default>(forward->getRenderPass());
        programs.push_back(std::move(program));

        return static_cast<uint>(programs.size() - 1);
    }

    uint Application::createPhongProgram()
    {
        auto program = std::make_unique<Programs::Phong>(forward->getRenderPass());
        programs.push_back(std::move(program));

        return static_cast<uint>(programs.size() - 1);
    }

}
