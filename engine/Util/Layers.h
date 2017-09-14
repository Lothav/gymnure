#ifndef OBSIDIAN2D_CORE_LAYERS_H
#define OBSIDIAN2D_CORE_LAYERS_H

#include <iostream>
#include "Util/Util.h"

namespace Gymnure
{
	namespace Util
	{

		typedef struct{
			VkLayerProperties properties;
			std::vector<VkExtensionProperties> extensions;
		} LayerProperties;


		class Layers : public Util
		{

		private:

			const std::vector<const char *> DESIRED_LAYERS = {
					"VK_LAYER_LUNARG_standard_validation",
					"VK_LAYER_LUNARG_object_tracker"
			};

		public:

			Layers()
			{
				this->setGlobalLayerProperties();
			}

			std::vector<LayerProperties> getInstanceLayerProps()
			{
				return _instanceLayerProps;
			}

			std::vector<const char*> getLayerNames()
			{
				std::cout << "Layers available:" << std::endl;
				std::vector<const char *> _layer_names;
				for(auto i : this->_instanceLayerProps)
				{
					std::cout << i.properties.layerName << std::endl;
					for(auto j : DESIRED_LAYERS)
					{
						if(0 == strcmp(j, i.properties.layerName))
						{
							_layer_names.push_back(j);
						}
					}
				}

				std::cout << "Using Layers:" << std::endl;
				for(auto i : _layer_names) {
					std::cout << i << std::endl;
				}

				return _layer_names;
			}

		private:

			std::vector<LayerProperties> _instanceLayerProps;

			VkResult setGlobalLayerProperties()
			{
				uint32_t instance_layer_count;
				VkLayerProperties *vk_props = NULL;
				VkResult res;

				do {
					res = vkEnumerateInstanceLayerProperties(&instance_layer_count, NULL);
					if (res) return res;
					if (instance_layer_count == 0) return VK_SUCCESS;
					vk_props = (VkLayerProperties *)realloc(vk_props, instance_layer_count * sizeof(VkLayerProperties));
					res = vkEnumerateInstanceLayerProperties(&instance_layer_count, vk_props);
				} while (res == VK_INCOMPLETE);

				for (uint32_t i = 0; i < instance_layer_count; i++) {
					LayerProperties layer_props;
					layer_props.properties = vk_props[i];
					res = this->setGlobalExtensionProperties(layer_props);
					if (res) return res;
					this->_instanceLayerProps.push_back(layer_props);
				}
				free(vk_props);
				assert(res == VK_SUCCESS);

				return res;
			}


			VkResult setGlobalExtensionProperties(LayerProperties &layer_props)
			{
				VkExtensionProperties *instance_extensions;
				uint32_t instance_extension_count;
				VkResult res;
				char *layer_name = NULL;
				layer_name = layer_props.properties.layerName;

				do {
					res = vkEnumerateInstanceExtensionProperties(layer_name, &instance_extension_count, NULL);
					if (res) return res;
					if (instance_extension_count == 0) return VK_SUCCESS;
					layer_props.extensions.resize(instance_extension_count);
					instance_extensions = layer_props.extensions.data();
					res = vkEnumerateInstanceExtensionProperties(layer_name, &instance_extension_count, instance_extensions);

				} while (res == VK_INCOMPLETE);

				return res;
			}

		};
	}
}
#endif //OBSIDIAN2D_CORE_LAYERS_H
