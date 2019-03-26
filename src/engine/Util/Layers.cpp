#include "Layers.h"
#include "Debug.hpp"

namespace Engine
{
    namespace Util
    {
        std::vector<LayerProperties> Layers::instanceLayerProps_ = {};

        std::vector<const char*> Layers::getLayerNames()
        {
            std::vector<const char *> layer_names_ = {};

            auto DESIRED_LAYERS = {
                "VK_LAYER_LUNARG_object_tracker",
                "VK_LAYER_GOOGLE_unique_objects",
                "VK_LAYER_LUNARG_assistant_layer",
                "VK_LAYER_LUNARG_core_validation",
                //"VK_LAYER_LUNARG_api_dump", // spam layer
                "VK_LAYER_LUNARG_parameter_validation",
                "VK_LAYER_LUNARG_screenshot",
                "VK_LAYER_LUNARG_standard_validation",
                "VK_LAYER_GOOGLE_threading",
                "VK_LAYER_LUNARG_monitor"
            };

            setGlobalLayerProperties();

            Debug::logInfo("Layers available:");
            for(auto i : instanceLayerProps_)
            {
                Debug::logInfo(i.properties.layerName);
                #ifdef DEBUG
                for(auto j : DESIRED_LAYERS)
                {
                    if(0 == strcmp(j, i.properties.layerName))
                    {
                        layer_names_.push_back(j);
                    }
                }
                #endif
            }

            Debug::logInfo("Using Layers:");
            for(auto i : layer_names_) {
                Debug::logInfo(i);
            }

            return layer_names_;
        }

        vk::Result Layers::setGlobalLayerProperties()
        {
            uint32_t instance_layer_count;
            vk::LayerProperties *vk_props = nullptr;
            vk::Result res;

            do {
                res = vk::enumerateInstanceLayerProperties(&instance_layer_count, nullptr, {});
                if (instance_layer_count == 0)
                    return vk::Result::eSuccess;
                vk_props = (vk::LayerProperties *)realloc(vk_props, instance_layer_count * sizeof(vk::LayerProperties));
                res = vk::enumerateInstanceLayerProperties(&instance_layer_count, vk_props);
            } while (res == vk::Result::eIncomplete);

            instanceLayerProps_.clear();
            for (uint32_t i = 0; i < instance_layer_count; i++) {
                LayerProperties layer_props;
                layer_props.properties = vk_props[i];
                res = setGlobalExtensionProperties(layer_props);
                instanceLayerProps_.push_back(layer_props);
            }

            free(vk_props);
            assert(res == vk::Result::eSuccess);

            return res;
        }

        vk::Result Layers::setGlobalExtensionProperties(LayerProperties &layer_props)
        {
            vk::ExtensionProperties *instance_extensions;
            uint32_t instance_extension_count;
            vk::Result res;
            char *layer_name = nullptr;
            layer_name = layer_props.properties.layerName;

            do {
                res = vk::enumerateInstanceExtensionProperties(layer_name, &instance_extension_count, nullptr);
                if (instance_extension_count == 0)
                    return vk::Result::eSuccess;
                layer_props.extensions.resize(instance_extension_count);
                instance_extensions = layer_props.extensions.data();
                res = vk::enumerateInstanceExtensionProperties(layer_name, &instance_extension_count, instance_extensions);
            } while (res == vk::Result::eIncomplete);

            return res;
        }

    }
}
