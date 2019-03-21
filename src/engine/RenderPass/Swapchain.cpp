//
// Created by luizorv on 9/3/17.
//

#include "Swapchain.h"

namespace Engine
{
    namespace RenderPass
    {
        SwapChain::SwapChain()
        {
            auto app_data = ApplicationData::data;

            vk::Result res;

            // Get supported format
            {
                uint32_t formatCount;

                res = app_data->gpu.getSurfaceFormatsKHR(app_data->surface, &formatCount, nullptr, {});
                assert(res == vk::Result::eSuccess);

                std::vector<vk::SurfaceFormatKHR> surfFormats(formatCount);
                res = app_data->gpu.getSurfaceFormatsKHR(app_data->surface, &formatCount, surfFormats.data());
                assert(res == vk::Result::eSuccess);
                // If the format list includes just one entry of vk::Format::eUndefined,
                // the surface has no preferred format.  Otherwise, at least one
                // supported format will be returned.
                if (formatCount == 1 && surfFormats[0].format == vk::Format::eUndefined) {
                    format_ = vk::Format::eB8G8R8A8Unorm;
                } else {

                    // iterate over the list of available surface format and
                    // check for the presence of vk::Format::eB8G8R8A8Unorm
                    bool found_B8G8R8A8_UNORM = false;
                    for (auto&& surfaceFormat : surfFormats)
                    {
                        if (surfaceFormat.format == vk::Format::eB8G8R8A8Unorm)
                        {
                            format_ = surfaceFormat.format;
                            colorSpace_ = surfaceFormat.colorSpace;
                            found_B8G8R8A8_UNORM = true;
                            break;
                        }
                    }

                    // in case vk::Format::eB8G8R8A8Unorm is not available
                    // select the first available color format
                    if (!found_B8G8R8A8_UNORM)
                    {
                        format_ = surfFormats[0].format;
                        colorSpace_ = surfFormats[0].colorSpace;
                    }
                }
            }

            vk::SwapchainCreateInfoKHR swapChainCI = buildSwapChainCI();

            res = app_data->device.createSwapchainKHR(&swapChainCI, nullptr, &swap_chain_);
            assert(res == vk::Result::eSuccess);

            res = app_data->device.getSwapchainImagesKHR(swap_chain_, &image_count_, nullptr, {});
            assert(res == vk::Result::eSuccess);

            auto* swap_chain_images_ = (vk::Image *) malloc(image_count_ * sizeof(vk::Image));
            assert(swap_chain_images_);

            res = app_data->device.getSwapchainImagesKHR(swap_chain_, &image_count_, swap_chain_images_);
            assert(res == vk::Result::eSuccess && image_count_ > 0);

            // Create Swapchain Buffer
            for (uint32_t i = 0; i < image_count_; i++)
            {
                ImageProps img_props = {};
                img_props.format 	  = format_;
                img_props.aspectMask  = vk::ImageAspectFlagBits::eColor;
                img_props.component.r = vk::ComponentSwizzle::eR;
                img_props.component.g = vk::ComponentSwizzle::eG;
                img_props.component.b = vk::ComponentSwizzle::eB;
                img_props.component.a = vk::ComponentSwizzle::eA;

                auto* sc_buffer = new Memory::BufferImage(img_props, swap_chain_images_[i]);

                swap_chain_buffer_.push_back(sc_buffer);
            }
        }

        SwapChain::~SwapChain()
        {
            if(swap_chain_buffer_.size() == image_count_) {
                for (u_int32_t i = 0; i < image_count_; i++){
                    swap_chain_buffer_[i]->image = nullptr;
                    delete swap_chain_buffer_[i];
                }
            }
            vkDestroySwapchainKHR(ApplicationData::data->device, swap_chain_, nullptr);
        }

        uint32_t SwapChain::getImageCount() const
        {
            return image_count_;
        }

        Memory::BufferImage* SwapChain::getSwapChainBuffer(uint32_t i) const
        {
            return swap_chain_buffer_[i];
        }

        vk::Format SwapChain::getSwapChainFormat() const
        {
            return format_;
        }

        vk::SwapchainKHR SwapChain::getSwapChainKHR() const
        {
            return swap_chain_;
        }

        vk::Queue SwapChain::getGraphicQueue() const
        {
            return graphics_queue_;
        }

        vk::Queue SwapChain::getPresentQueue() const
        {
            return present_queue_;
        }

        vk::SwapchainCreateInfoKHR SwapChain::buildSwapChainCI()
        {
            auto app_data = ApplicationData::data;

            vk::Result res;
            uint32_t graphics_queue_family_index_ = UINT32_MAX;
            uint32_t present_queue_family_index_  = UINT32_MAX;

            auto *pSupportsPresent = (vk::Bool32 *)malloc(app_data->queue_family_count * sizeof(vk::Bool32));

            for (uint32_t i = 0; i < app_data->queue_family_count; i++) {
                app_data->gpu.getSurfaceSupportKHR(i, app_data->surface, &pSupportsPresent[i]);
            }

            // Search for a graphics and a present queue in the array of queue
            // families, try to find one that supports both

            for (uint32_t i = 0; i < app_data->queue_family_count; ++i) {
                if ((app_data->queue_family_props[i].queueFlags & vk::QueueFlagBits::eGraphics)) {
                    if (graphics_queue_family_index_ == UINT32_MAX) graphics_queue_family_index_ = i;

                    if (pSupportsPresent[i] == VK_TRUE) {
                        graphics_queue_family_index_ = i;
                        present_queue_family_index_ = i;
                        break;
                    }
                }
            }

            if (present_queue_family_index_ == UINT32_MAX) {
                // If didn't find a queue that supports both graphics and present, then
                // find a separate present queue.
                for (size_t i = 0; i < app_data->queue_family_count; ++i)
                    if (pSupportsPresent[i] == VK_TRUE) {
                        present_queue_family_index_ = (u_int32_t)i;
                        break;
                    }
            }
            free(pSupportsPresent);

            // Generate error if could not find queues that support graphics
            // and present
            if (graphics_queue_family_index_ == UINT32_MAX || present_queue_family_index_ == UINT32_MAX) {
                Debug::logError("Could not find a queues for both graphics and present");
            }

            app_data->device.getQueue(graphics_queue_family_index_, 0, &graphics_queue_);
            if (graphics_queue_family_index_ == present_queue_family_index_) {
                present_queue_ = graphics_queue_;
            } else {
                app_data->device.getQueue(present_queue_family_index_, 0, &present_queue_);
            }

            vk::SurfaceCapabilitiesKHR surfCapabilities;
            res = app_data->gpu.getSurfaceCapabilitiesKHR(app_data->surface, &surfCapabilities);
            assert(res == vk::Result::eSuccess);

            uint32_t presentation_modes_count = 0;

            res = app_data->gpu.getSurfacePresentModesKHR(app_data->surface, &presentation_modes_count, nullptr, {});
            assert(res == vk::Result::eSuccess);
            auto *presentModes = (vk::PresentModeKHR *)malloc(presentation_modes_count * sizeof(vk::PresentModeKHR));
            assert(presentModes);
            res = app_data->gpu.getSurfacePresentModesKHR(app_data->surface, &presentation_modes_count, presentModes);
            assert(res == vk::Result::eSuccess);

            vk::Extent2D swapchainExtent;
            // width and height are either both 0xFFFFFFFF, or both not 0xFFFFFFFF.
            if (surfCapabilities.currentExtent.width == 0xFFFFFFFF) {
                // If the surface size is undefined, the size is set to
                // the size of the images requested.
                swapchainExtent.width  = app_data->view_width;
                swapchainExtent.height = app_data->view_height;
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
            vk::PresentModeKHR swapchainPresentMode = vk::PresentModeKHR::eFifo;

            // Determine the number of vk::Image's to use in the swap chain.
            // We need to acquire only 1 presentable image at at time.
            // Asking for minImageCount images ensures that we can acquire
            // 1 presentable image as long as we present it before attempting
            // to acquire another.
            image_count_ = surfCapabilities.minImageCount + 1;
            if ((surfCapabilities.maxImageCount > 0) && (image_count_ > surfCapabilities.maxImageCount))
            {
                image_count_ = surfCapabilities.maxImageCount;
            }

            vk::SurfaceTransformFlagBitsKHR preTransform;
            if (surfCapabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity) {
                preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
            } else {
                preTransform = surfCapabilities.currentTransform;
            }

            // Find a supported composite alpha mode - one of these is guaranteed to be set
            vk::CompositeAlphaFlagBitsKHR compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
            vk::CompositeAlphaFlagBitsKHR compositeAlphaFlags[4] = {
                vk::CompositeAlphaFlagBitsKHR::eOpaque,
                vk::CompositeAlphaFlagBitsKHR::ePreMultiplied,
                vk::CompositeAlphaFlagBitsKHR::ePostMultiplied,
                vk::CompositeAlphaFlagBitsKHR::eInherit,
            };
            for (uint32_t i = 0; i < sizeof(compositeAlphaFlags); i++) {
                if (surfCapabilities.supportedCompositeAlpha & compositeAlphaFlags[i]) {
                    compositeAlpha = compositeAlphaFlags[i];
                    break;
                }
            }

            vk::SwapchainCreateInfoKHR swapchain_ci = {};
            swapchain_ci.pNext 					= nullptr;
            swapchain_ci.surface 				= app_data->surface;
            swapchain_ci.minImageCount 			= image_count_;
            swapchain_ci.imageFormat 			= format_;
            swapchain_ci.imageColorSpace		= colorSpace_;
            swapchain_ci.imageUsage 			= vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eColorAttachment;
            swapchain_ci.imageSharingMode 		= vk::SharingMode::eExclusive;
            swapchain_ci.imageExtent.width 		= swapchainExtent.width;
            swapchain_ci.imageExtent.height 	= swapchainExtent.height;
            swapchain_ci.preTransform 			= preTransform;
            swapchain_ci.compositeAlpha 		= compositeAlpha;
            swapchain_ci.imageArrayLayers 		= 1;
            swapchain_ci.presentMode 			= swapchainPresentMode;
            swapchain_ci.oldSwapchain 			= nullptr;
            swapchain_ci.clipped 				= VK_TRUE;
            swapchain_ci.queueFamilyIndexCount  = 0;
            swapchain_ci.pQueueFamilyIndices 	= nullptr;

            auto* queueFamilyIndices = static_cast<uint32_t*>(malloc(sizeof(uint32_t) * 2));
            queueFamilyIndices[0] = (uint32_t)graphics_queue_family_index_;
            queueFamilyIndices[1] = (uint32_t)present_queue_family_index_;

            if (graphics_queue_family_index_ != present_queue_family_index_) {
                // If the graphics and present queues are from different queue families,
                // we either have to explicitly transfer ownership of images between the
                // queues, or we have to create the swapchain with imageSharingMode
                // as VK_SHARING_MODE_CONCURRENT
                swapchain_ci.imageSharingMode 		= vk::SharingMode::eConcurrent;
                swapchain_ci.queueFamilyIndexCount 	= 2;
                swapchain_ci.pQueueFamilyIndices 	= queueFamilyIndices;
            }

            return swapchain_ci;
        }

    }
}

