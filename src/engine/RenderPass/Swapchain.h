//
// Created by luizorv on 9/3/17.
//

#ifndef OBSIDIAN2D_SWAPCHAIN_H
#define OBSIDIAN2D_SWAPCHAIN_H

#include <iostream>
#include "Descriptors/Textures.h"
#include "Memory/BufferImage.h"
#include <vector>
#include <vulkan/vulkan.h>

struct SwapChainParams {
	std::vector<VkQueueFamilyProperties,
			mem::StdAllocator<
					VkQueueFamilyProperties>>     	queue_family_props;
	uint32_t 										queue_family_count;
	VkSurfaceKHR 									surface;
	VkPhysicalDevice 								gpu;
	VkDevice 										device;
	uint32_t 										width;
	uint32_t 										height;
	VkPhysicalDeviceMemoryProperties 				memory_props;
};

namespace Engine
{
	namespace RenderPass
	{
		class SwapChain {

		private:

			uint32_t 							_image_count{};
			VkSwapchainKHR  					_swap_chain = nullptr;
			VkQueue 							_graphics_queue{}, _present_queue{};
			VkFormat 							format;

			struct SwapChainParams 				_swap_chain_params 	  = {};

			std::vector<Memory::BufferImage *> 	_swap_chain_buffer = {};

		public:

			explicit SwapChain(struct SwapChainParams&& swap_chain_params)
			{
				_swap_chain_params = swap_chain_params;

				VkResult res;
				VkImage* _swap_chain_images = nullptr;

                // Get supported format
                {
                    uint32_t formatCount;

                    res = vkGetPhysicalDeviceSurfaceFormatsKHR(_swap_chain_params.gpu, _swap_chain_params.surface, &formatCount, nullptr);
                    assert(res == VK_SUCCESS);
                    auto *surfFormats = (VkSurfaceFormatKHR *)malloc(formatCount * sizeof(VkSurfaceFormatKHR));
                    res = vkGetPhysicalDeviceSurfaceFormatsKHR(_swap_chain_params.gpu, _swap_chain_params.surface, &formatCount, surfFormats);
                    assert(res == VK_SUCCESS);
                    // If the format list includes just one entry of VK_FORMAT_UNDEFINED,
                    // the surface has no preferred format.  Otherwise, at least one
                    // supported format will be returned.
                    if (formatCount == 1 && surfFormats[0].format == VK_FORMAT_UNDEFINED) {
                        format = VK_FORMAT_B8G8R8A8_UNORM;
                    } else {
                        assert(formatCount >= 1);
                        format = surfFormats[0].format;
                    }
                    free(surfFormats);
                }

                VkSwapchainCreateInfoKHR swapChainCI = getSwapChainCI();

				res = vkCreateSwapchainKHR(_swap_chain_params.device, &swapChainCI, nullptr, &_swap_chain);
				assert(res == VK_SUCCESS);

				res = vkGetSwapchainImagesKHR(_swap_chain_params.device, _swap_chain, &_image_count, nullptr);
				assert(res == VK_SUCCESS);

				_swap_chain_images = (VkImage *)malloc(_image_count * sizeof(VkImage));
				assert(_swap_chain_images);

				res = vkGetSwapchainImagesKHR(_swap_chain_params.device, _swap_chain, &_image_count, _swap_chain_images);
				assert(res == VK_SUCCESS && _image_count > 0);

				// Create Swapchain Buffer
				for (uint32_t i = 0; i < _image_count; i++)
				{
					ImageProps img_props = {};
					img_props.format = format;
					img_props.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

					MemoryProps mem_props = {};
					mem_props.device = _swap_chain_params.device;

					auto* sc_buffer = new Memory::BufferImage(mem_props, img_props, &_swap_chain_images[i]);

					_swap_chain_buffer.push_back(sc_buffer);
				}
			}

			~SwapChain()
			{
				if(_swap_chain_buffer.size() == _image_count) {
					for (u_int32_t i = 0; i < _image_count; i++){
						_swap_chain_buffer[i]->image = nullptr;
						delete _swap_chain_buffer[i];
					}
				}
				vkDestroySwapchainKHR(_swap_chain_params.device, _swap_chain, nullptr);
			}

			void* operator new(std::size_t size)
			{
				return mem::Provider::getMemory(size);
			}

			void operator delete(void* ptr)
			{
				// Do not free memory here!
			}

			uint32_t getImageCount() const
			{
				return _image_count;
			}

			Memory::BufferImage * getSwapChainBuffer(uint32_t i) const
			{
				return _swap_chain_buffer[i];
			}

			VkFormat getSwapChainFormat() const
			{
				return format;
			}

			VkSwapchainKHR getSwapChainKHR() const
			{
				return _swap_chain;
			}

			VkQueue getGraphicQueue() const
			{
				return _graphics_queue;
			}

			VkQueue getPresentQueue() const
			{
				return _present_queue;
			}

		private:

			VkSwapchainCreateInfoKHR getSwapChainCI()
			{
				VkResult res;
				uint32_t _graphics_queue_family_index = UINT32_MAX;
				uint32_t _present_queue_family_index  = UINT32_MAX;

				VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
				auto *pSupportsPresent = (VkBool32 *)malloc(_swap_chain_params.queue_family_count * sizeof(VkBool32));

				for (uint32_t i = 0; i < _swap_chain_params.queue_family_count; i++) {
					vkGetPhysicalDeviceSurfaceSupportKHR(_swap_chain_params.gpu, i, _swap_chain_params.surface, &pSupportsPresent[i]);
				}

				// Search for a graphics and a present queue in the array of queue
				// families, try to find one that supports both

				for (uint32_t i = 0; i < _swap_chain_params.queue_family_count; ++i) {
					if ((_swap_chain_params.queue_family_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
						if (_graphics_queue_family_index == UINT32_MAX) _graphics_queue_family_index = i;

						if (pSupportsPresent[i] == VK_TRUE) {
							_graphics_queue_family_index = i;
							_present_queue_family_index = i;
							break;
						}
					}
				}

				if (_present_queue_family_index == UINT32_MAX) {
					// If didn't find a queue that supports both graphics and present, then
					// find a separate present queue.
					for (size_t i = 0; i < _swap_chain_params.queue_family_count; ++i)
						if (pSupportsPresent[i] == VK_TRUE) {
							_present_queue_family_index = (u_int32_t)i;
							break;
						}
				}
				free(pSupportsPresent);

				// Generate error if could not find queues that support graphics
				// and present
				if (_graphics_queue_family_index == UINT32_MAX || _present_queue_family_index == UINT32_MAX) {
					std::cout << "Could not find a queues for both graphics and present";
					exit(-1);
				}

				VkCommandBufferBeginInfo cmd_buf_info = {};
				cmd_buf_info.sType 							= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				cmd_buf_info.pNext 							= nullptr;
				cmd_buf_info.flags 							= 0;
				cmd_buf_info.pInheritanceInfo 				= nullptr;

				vkGetDeviceQueue(_swap_chain_params.device, _graphics_queue_family_index, 0, &_graphics_queue);
				if (_graphics_queue_family_index == _present_queue_family_index) {
					_present_queue = _graphics_queue;
				} else {
					vkGetDeviceQueue(_swap_chain_params.device, _present_queue_family_index, 0, &_present_queue);
				}

				VkSurfaceCapabilitiesKHR surfCapabilities;
				res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_swap_chain_params.gpu, _swap_chain_params.surface, &surfCapabilities);
				assert(res == VK_SUCCESS);

				res = vkGetPhysicalDeviceSurfacePresentModesKHR(_swap_chain_params.gpu, _swap_chain_params.surface, &_swap_chain_params.queue_family_count, nullptr);
				assert(res == VK_SUCCESS);
				auto *presentModes = (VkPresentModeKHR *)malloc(_swap_chain_params.queue_family_count * sizeof(VkPresentModeKHR));
				assert(presentModes);
				res = vkGetPhysicalDeviceSurfacePresentModesKHR(_swap_chain_params.gpu, _swap_chain_params.surface, &_swap_chain_params.queue_family_count, presentModes);
				assert(res == VK_SUCCESS);

				VkExtent2D swapchainExtent;
				// width and height are either both 0xFFFFFFFF, or both not 0xFFFFFFFF.
				if (surfCapabilities.currentExtent.width == 0xFFFFFFFF) {
					// If the surface size is undefined, the size is set to
					// the size of the images requested.
					swapchainExtent.width  = _swap_chain_params.width;
					swapchainExtent.height = _swap_chain_params.height;
					if (swapchainExtent.width < surfCapabilities.minImageExtent.width) {
						swapchainExtent.width = surfCapabilities.minImageExtent.width;
					} else if (swapchainExtent.width > surfCapabilities.maxImageExtent.width) {
						swapchainExtent.width = surfCapabilities.maxImageExtent.width;
					}

					if (swapchainExtent.height < surfCapabilities.minImageExtent.height) {
						swapchainExtent.height = surfCapabilities.minImageExtent.height;
					} else if (swapchainExtent.height > surfCapabilities.maxImageExtent.height) {
						swapchainExtent.height = surfCapabilities.maxImageExtent.height;
					}
				} else {
					// If the surface size is defined, the swap chain size must match
					swapchainExtent = surfCapabilities.currentExtent;
				}

				// The FIFO present mode is guaranteed by the spec to be supported
				// Also note that current Android driver only supports FIFO
				VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

				// Determine the number of VkImage's to use in the swap chain.
				// We need to acquire only 1 presentable image at at time.
				// Asking for minImageCount images ensures that we can acquire
				// 1 presentable image as long as we present it before attempting
				// to acquire another.
				_image_count = surfCapabilities.minImageCount + 1;
				if ((surfCapabilities.maxImageCount > 0) && (_image_count > surfCapabilities.maxImageCount))
				{
					_image_count = surfCapabilities.maxImageCount;
				}

				VkSurfaceTransformFlagBitsKHR preTransform;
				if (surfCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
					preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
				} else {
					preTransform = surfCapabilities.currentTransform;
				}

				// Find a supported composite alpha mode - one of these is guaranteed to be set
				VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
				VkCompositeAlphaFlagBitsKHR compositeAlphaFlags[4] = {
						VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
						VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
						VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
						VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
				};
				for (uint32_t i = 0; i < sizeof(compositeAlphaFlags); i++) {
					if (surfCapabilities.supportedCompositeAlpha & compositeAlphaFlags[i]) {
						compositeAlpha = compositeAlphaFlags[i];
						break;
					}
				}

				VkSwapchainCreateInfoKHR swapchain_ci = {};
				swapchain_ci.sType 					= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
				swapchain_ci.pNext 					= nullptr;
				swapchain_ci.surface 				= _swap_chain_params.surface;
				swapchain_ci.minImageCount 			= _image_count;
				swapchain_ci.imageFormat 			= format;
				swapchain_ci.imageExtent.width 		= swapchainExtent.width;
				swapchain_ci.imageExtent.height 	= swapchainExtent.height;
				swapchain_ci.preTransform 			= preTransform;
				swapchain_ci.compositeAlpha 		= compositeAlpha;
				swapchain_ci.imageArrayLayers 		= 1;
				swapchain_ci.presentMode 			= swapchainPresentMode;
				swapchain_ci.oldSwapchain 			= VK_NULL_HANDLE;
				swapchain_ci.clipped 				= (VkBool32)true;
				swapchain_ci.imageColorSpace 		= VK_COLORSPACE_SRGB_NONLINEAR_KHR;
				swapchain_ci.imageUsage 			= usageFlags;
				swapchain_ci.imageSharingMode 		= VK_SHARING_MODE_EXCLUSIVE;
				swapchain_ci.queueFamilyIndexCount  = 0;
				swapchain_ci.pQueueFamilyIndices 	= nullptr;

				auto* queueFamilyIndices = static_cast<uint32_t*>(malloc(sizeof(uint32_t) * 2));
				queueFamilyIndices[0] = (uint32_t)_graphics_queue_family_index;
				queueFamilyIndices[1] = (uint32_t)_present_queue_family_index;

				if (_graphics_queue_family_index != _present_queue_family_index) {
					// If the graphics and present queues are from different queue families,
					// we either have to explicitly transfer ownership of images between the
					// queues, or we have to create the swapchain with imageSharingMode
					// as VK_SHARING_MODE_CONCURRENT
					swapchain_ci.imageSharingMode 		= VK_SHARING_MODE_CONCURRENT;
					swapchain_ci.queueFamilyIndexCount 	= 2;
					swapchain_ci.pQueueFamilyIndices 	= queueFamilyIndices;
				}

				return swapchain_ci;
			}

		};
	}
}
#endif //OBSIDIAN2D_SWAPCHAIN_H
