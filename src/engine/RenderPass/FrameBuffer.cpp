#include "FrameBuffer.h"

namespace Engine
{
    namespace RenderPass
    {
        FrameBuffer::FrameBuffer()
        {
            swap_chain_ = std::make_shared<SwapChain>();

            auto app_data = ApplicationData::data;

            vk::FormatProperties props;
            vk::ImageTiling depth_tiling;

            app_data->gpu.getFormatProperties(depth_format_, &props);

            if (props.linearTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment) {
                depth_tiling = vk::ImageTiling::eLinear;
            } else if (props.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment) {
                depth_tiling = vk::ImageTiling::eOptimal;
            } else {
                /* Try other depth formats? */
                assert(false);
            }

            struct ImageProps img_props = {};
            img_props.format 		= depth_format_;
            img_props.tiling 		= depth_tiling;
            img_props.aspectMask 	= vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
            img_props.usage 		= vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eDepthStencilAttachment;
            img_props.width 		= app_data->view_width;
            img_props.height 		= app_data->view_height;

            struct MemoryProps mem_props = {};
            mem_props.props_flags   = vk::MemoryPropertyFlagBits::eDeviceLocal;

            // Create Depth Buffer
            depth_buffer_ = std::make_unique<Memory::BufferImage>(mem_props, img_props);
        }

        virtual FrameBuffer::~FrameBuffer()
        {
            for (auto &frame_buffer_ : frame_buffers_)
                vkDestroyFramebuffer(ApplicationData::data->device, frame_buffer_, nullptr);
        }

        void FrameBuffer::createFrameBuffer(vk::RenderPass render_pass)
        {
            auto app_data = ApplicationData::data;

            vk::ImageView img_attachments[2];
            img_attachments[1] = depth_buffer_->view;

            vk::FramebufferCreateInfo fb_info = {};
            fb_info.pNext 					= nullptr;
            fb_info.renderPass 				= render_pass;
            fb_info.attachmentCount 		= 2;
            fb_info.pAttachments 			= img_attachments;
            fb_info.width 					= app_data->view_width;
            fb_info.height 					= app_data->view_height;
            fb_info.layers 					= 1;

            frame_buffers_.resize(swap_chain_->getImageCount());

            for (uint32_t i = 0; i < swap_chain_->getImageCount(); i++) {
                img_attachments[0] = (swap_chain_->getSwapChainBuffer(i))->view;

                vk::Result res = app_data->device.createFramebuffer(&fb_info, nullptr, &frame_buffers_[i]);
                assert(res == vk::Result::eSuccess);
            }
        }

        const vk::Format FrameBuffer::getDepthBufferFormat() const
        {
            return depth_format_;
        }

        std::shared_ptr<SwapChain> FrameBuffer::getSwapChain()
        {
            return swap_chain_;
        }

        std::vector<vk::Framebuffer> FrameBuffer::getFrameBuffers()
        {
            return frame_buffers_;
        }

    }
}

