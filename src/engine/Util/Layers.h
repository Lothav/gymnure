#ifndef OBSIDIAN2D_CORE_LAYERS_H
#define OBSIDIAN2D_CORE_LAYERS_H

#include <iostream>
#include <Provider.hpp>
#include "Util/Util.h"

namespace Engine
{
    namespace Util
    {
        typedef struct{
            VkLayerProperties properties = {};
            std::vector<VkExtensionProperties> extensions = {};
        } LayerProperties;

        class Layers
        {

        private:

        public:

            Layers() = delete;

            static std::vector<const char*> getLayerNames();

        private:

            static std::vector<LayerProperties> instanceLayerProps_;

            static VkResult setGlobalLayerProperties();
            static VkResult setGlobalExtensionProperties(LayerProperties &layer_props);
        };

    }
}
#endif //OBSIDIAN2D_CORE_LAYERS_H
