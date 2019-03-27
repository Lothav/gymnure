#include "FrameBuffer.h"

namespace Engine
{
    namespace RenderPass
    {
        FrameBuffer::FrameBuffer()
        {
            auto app_data = ApplicationData::data;

            swap_chain_ = std::make_shared<SwapChain>();

            // Create Depth Buffer
            {
                // Since all depth formats may be optional, we need to find a suitable depth format to use
                // Start with the highest precision packed format
                std::vector<vk::Format> depthFormats = {
                    vk::Format::eD32SfloatS8Uint,
                    vk::Format::eD32Sfloat,
                    vk::Format::eD24UnormS8Uint,
                    vk::Format::eD16UnormS8Uint,
                    vk::Format::eD16Unorm,
                };

                for (auto& format : depthFormats)
                {
                    vk::FormatProperties formatProps = app_data->gpu.getFormatProperties(format);
                    // Format must support depth stencil attachment for optimal tiling
                    if (formatProps.optimalTilingFeatures &  vk::FormatFeatureFlagBits::eDepthStencilAttachment)
                    {
                        depth_format_ = format;
                        break;
                    }
                }

                if(depth_format_ == vk::Format::eUndefined)
                    throw "Cannot find an optimal valid Depth format!";

                Memory::ImageViewProps img_view_props = {};
                img_view_props.format = depth_format_;
                img_view_props.aspectMask = vk::ImageAspectFlagBits::eDepth;

                Memory::ImageProps img_props = {};
                img_props.width             = static_cast<uint32_t>(app_data->view_width);
                img_props.height            = static_cast<uint32_t>(app_data->view_height);
                img_props.usage             = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eDepthStencilAttachment;
                img_props.tiling            = vk::ImageTiling::eOptimal;
                img_props.image_props_flags = vk::MemoryPropertyFlagBits::eDeviceLocal;

                // Create Depth Buffer
                depth_buffer_ = std::make_unique<Memory::BufferImage>(img_view_props, img_props);
            }

            // Create Render Pass
            {
                std::vector<struct rpAttachments> rp_attachments = {};

                struct rpAttachments attch = {};
                attch.format = getSwapChain()->getSwapChainFormat();
                attch.clear  = true;
                rp_attachments.push_back(attch);

                attch.format = getDepthBufferFormat();
                attch.clear  = true;
                rp_attachments.push_back(attch);

                render_pass_ = std::make_shared<RenderPass>(rp_attachments);
            }

            // Create Frame Buffers
            {
                vk::ImageView img_attachments[2];
                img_attachments[1] = depth_buffer_->view;

                vk::FramebufferCreateInfo fb_info = {};
                fb_info.pNext 					= nullptr;
                fb_info.renderPass 				= render_pass_->getRenderPass();
                fb_info.attachmentCount 		= 2;
                fb_info.pAttachments 			= img_attachments;
                fb_info.width 					= app_data->view_width;
                fb_info.height 					= app_data->view_height;
                fb_info.layers 					= 1;

                frame_buffers_.resize(swap_chain_->getImageCount());

                for (uint32_t i = 0; i < swap_chain_->getImageCount(); i++) {
                    img_attachments[0] = swap_chain_->getSwapChainImageView(i);

                    frame_buffers_[i] = app_data->device.createFramebuffer(fb_info);
                }
            }
        }

        FrameBuffer::~FrameBuffer()
        {
            for (auto &frame_buffer_ : frame_buffers_)
                vkDestroyFramebuffer(ApplicationData::data->device, frame_buffer_, nullptr);
        }

        const vk::Format FrameBuffer::getDepthBufferFormat() const
        {
            return depth_format_;
        }

        std::shared_ptr<SwapChain> FrameBuffer::getSwapChain() const
        {
            return swap_chain_;
        }

        uint32_t FrameBuffer::getImageCount() const
        {
            return static_cast<uint32_t>(frame_buffers_.size());
        }

        std::vector<vk::Framebuffer> FrameBuffer::getFrameBuffers() const
        {
            return frame_buffers_;
        }

    }
}

