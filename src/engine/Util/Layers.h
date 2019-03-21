#ifndef OBSIDIAN2D_CORE_LAYERS_H
#define OBSIDIAN2D_CORE_LAYERS_H

#include <vulkan/vulkan.hpp>
#include <iostream>
#include <Provider.hpp>
#include "Util/Util.h"

namespace Engine
{
    namespace Util
    {
        typedef struct{
            vk::LayerProperties properties = {};
            std::vector<vk::ExtensionProperties> extensions = {};
        } LayerProperties;

        class Layers
        {

        private:

        public:

            Layers() = delete;

            static std::vector<const char*> getLayerNames();

        private:

            static std::vector<LayerProperties> instanceLayerProps_;

            static vk::Result setGlobalLayerProperties();
            static vk::Result setGlobalExtensionProperties(LayerProperties &layer_props);
        };

    }
}
#endif //OBSIDIAN2D_CORE_LAYERS_H
