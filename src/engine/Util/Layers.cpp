#include "Layers.h"

namespace Engine
{
    namespace Util
    {
        std::vector<LayerProperties> Layers::instanceLayerProps_ = {};

        std::vector<const char*> Layers::getLayerNames()
        {
            std::vector<const char *> layer_names_ = {};
            #ifdef DEBUG

            auto DESIRED_LAYERS = {
                "VK_LAYER_LUNARG_object_tracker",
                "VK_LAYER_GOOGLE_unique_objects",
                "VK_LAYER_LUNARG_assistant_layer",
                "VK_LAYER_LUNARG_core_validation",
                "VK_LAYER_LUNARG_api_dump", // spam layer
                "VK_LAYER_LUNARG_parameter_validation",
                "VK_LAYER_LUNARG_screenshot",
                "VK_LAYER_LUNARG_standard_validation",
                "VK_LAYER_GOOGLE_threading",
                "VK_LAYER_LUNARG_monitor"
            };

            setGlobalLayerProperties();

            std::cout << "Layers available:" << std::endl;
            for(auto i : instanceLayerProps_)
            {
                std::cout << i.properties.layerName << std::endl;
                for(auto j : DESIRED_LAYERS)
                {
                    if(0 == strcmp(j, i.properties.layerName))
                    {
                        layer_names_.push_back(j);
                    }
                }
            }

            std::cout << "Using Layers:" << std::endl;
            for(auto i : layer_names_) {
                std::cout << i << std::endl;
            }
            #endif

            return layer_names_;
        }

        VkResult Layers::setGlobalLayerProperties()
        {
            uint32_t instance_layer_count;
            VkLayerProperties *vk_props = nullptr;
            VkResult res;

            do {
                res = vkEnumerateInstanceLayerProperties(&instance_layer_count, nullptr);
                if (res) return res;
                if (instance_layer_count == 0) return VK_SUCCESS;
                vk_props = (VkLayerProperties *)realloc(vk_props, instance_layer_count * sizeof(VkLayerProperties));
                res = vkEnumerateInstanceLayerProperties(&instance_layer_count, vk_props);
            } while (res == VK_INCOMPLETE);

            instanceLayerProps_.clear();
            for (uint32_t i = 0; i < instance_layer_count; i++) {
                LayerProperties layer_props;
                layer_props.properties = vk_props[i];
                res = setGlobalExtensionProperties(layer_props);
                if (res) return res;
                instanceLayerProps_.push_back(layer_props);
            }

            free(vk_props);
            assert(res == VK_SUCCESS);

            return res;
        }

        VkResult Layers::setGlobalExtensionProperties(LayerProperties &layer_props)
        {
            VkExtensionProperties *instance_extensions;
            uint32_t instance_extension_count;
            VkResult res;
            char *layer_name = nullptr;
            layer_name = layer_props.properties.layerName;

            do {
                res = vkEnumerateInstanceExtensionProperties(layer_name, &instance_extension_count, nullptr);
                if (res) return res;
                if (instance_extension_count == 0) return VK_SUCCESS;
                layer_props.extensions.resize(instance_extension_count);
                instance_extensions = layer_props.extensions.data();
                res = vkEnumerateInstanceExtensionProperties(layer_name, &instance_extension_count, instance_extensions);

            } while (res == VK_INCOMPLETE);

            return res;
        }

    }
}
