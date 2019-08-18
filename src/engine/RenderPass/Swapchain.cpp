#include <Memory/ImageFormats.hpp>
#include "Swapchain.h"
#include "Queue.h"

namespace Engine
{
    namespace RenderPass
    {
        std::shared_ptr<SwapChain> SwapChain::instance = nullptr;

        std::shared_ptr<SwapChain> SwapChain::getInstance()
        {
            if(instance == nullptr)
                instance = std::shared_ptr<SwapChain>(new SwapChain());
            return instance;
        }

        SwapChain::SwapChain()
        {
            auto app_data = ApplicationData::data;

            // Create SwapchainKHR
            {
                vk::SwapchainCreateInfoKHR swapChainCI = buildSwapChainCI();
                swap_chain_ = app_data->device.createSwapchainKHR(swapChainCI);
            }

            // Create Swapchain Image Buffer
            {
                vk::Result res = app_data->device.getSwapchainImagesKHR(swap_chain_, &image_count_, nullptr, {});
                assert(res == vk::Result::eSuccess && image_count_ > 0);

                auto* swap_chain_images_ = (vk::Image *) malloc(image_count_ * sizeof(vk::Image));
                assert(swap_chain_images_);

                res = app_data->device.getSwapchainImagesKHR(swap_chain_, &image_count_, swap_chain_images_);
                assert(res == vk::Result::eSuccess && image_count_ > 0);

                // Create Swapchain Buffer
                for (uint32_t i = 0; i < image_count_; i++)
                {
                    Memory::ImageProps img_props = {};
                    img_props.format = Memory::ImageFormats::getSurfaceFormat().format;
                    img_props.component = {
                        vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG,
                        vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA
                    };

                    swap_chain_buffer_.push_back(std::make_unique<Memory::BufferImage>(img_props, swap_chain_images_[i]));
                }
            }
        }

        void SwapChain::reset()
        {
            ApplicationData::data->device.destroySwapchainKHR(instance->swap_chain_);
            instance->swap_chain_buffer_.clear();
            instance.reset();
        }

        uint32_t SwapChain::getImageCount() const
        {
            return image_count_;
        }

        vk::ImageView SwapChain::getSwapChainImageView(uint32_t swapchain_index) const
        {
            return swap_chain_buffer_[swapchain_index]->view;
        }

        vk::SwapchainKHR SwapChain::getSwapChainKHR() const
        {
            return swap_chain_;
        }

        vk::SwapchainCreateInfoKHR SwapChain::buildSwapChainCI()
        {
            auto app_data = ApplicationData::data;

            vk::Result res;
            vk::SurfaceCapabilitiesKHR surfCapabilities;
            surfCapabilities = app_data->gpu.getSurfaceCapabilitiesKHR(app_data->surface);

            vk::PresentModeKHR swapchainPresentMode = vk::PresentModeKHR::eFifo;
            {
                std::vector<vk::PresentModeKHR> presentation_modes = {};
                uint32_t presentation_modes_count = 0;

                res = app_data->gpu.getSurfacePresentModesKHR(app_data->surface, &presentation_modes_count, nullptr,
                                                              {});
                assert(res == vk::Result::eSuccess && presentation_modes_count > 0);
                presentation_modes.resize(presentation_modes_count);
                res = app_data->gpu.getSurfacePresentModesKHR(app_data->surface, &presentation_modes_count,
                                                              presentation_modes.data());
                assert(res == vk::Result::eSuccess && presentation_modes_count > 0);

                // The FIFO present mode is guaranteed by the spec to be supported
                // Also note that current Android driver only supports FIFO
                for (short i = 0; i < presentation_modes_count; i++) {
                    // Use MailBox if supported
                    if (presentation_modes[i] == vk::PresentModeKHR::eMailbox) {
                        swapchainPresentMode = presentation_modes[i];
                        break;
                    }
                }
            }

            vk::Extent2D swapchainExtent;
            {
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
            }

            // Determine the number of vk::Image's to use in the swap chain.
            // We need to acquire only 1 presentable image at at time.
            // Asking for minImageCount images ensures that we can acquire
            // 1 presentable image as long as we present it before attempting
            // to acquire another.
            image_count_ = surfCapabilities.minImageCount + 1;
            if ((surfCapabilities.maxImageCount > 0) && (image_count_ > surfCapabilities.maxImageCount))
                image_count_ = surfCapabilities.maxImageCount;

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

            vk::SurfaceFormatKHR surface_format = Memory::ImageFormats::getSurfaceFormat();

            vk::SwapchainCreateInfoKHR swapchain_ci = {};
            swapchain_ci.pNext 					= nullptr;
            swapchain_ci.surface 				= app_data->surface;
            swapchain_ci.minImageCount 			= image_count_;
            swapchain_ci.imageFormat 			= surface_format.format;
            swapchain_ci.imageColorSpace		= surface_format.colorSpace;
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

            uint32_t graphics_queue_family_index_ = Queue::GetGraphicQueueIndex();
            uint32_t present_queue_family_index_  = Queue::GetPresentQueueIndex();

            
            if (graphics_queue_family_index_ != present_queue_family_index_) {
                // If the graphics and present queues are from different queue families,
                // we either have to explicitly transfer ownership of images between the
                // queues, or we have to create the swapchain with imageSharingMode
                // as VK_SHARING_MODE_CONCURRENT

                std::array<uint32_t, 2> queueFamilyIndices = {
                    (uint32_t)graphics_queue_family_index_,
                    (uint32_t)present_queue_family_index_
                };

                swapchain_ci.imageSharingMode 		= vk::SharingMode::eConcurrent;
                swapchain_ci.queueFamilyIndexCount 	= queueFamilyIndices.size();
                swapchain_ci.pQueueFamilyIndices 	= queueFamilyIndices.data();
            }

            return swapchain_ci;
        }

    }
}

