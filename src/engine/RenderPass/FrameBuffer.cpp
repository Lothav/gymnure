#include <Memory/ImageFormats.hpp>
#include "FrameBuffer.h"

namespace Engine
{
    namespace RenderPass
    {
        FrameBuffer::FrameBuffer(std::vector<vk::ImageView> img_attachments, std::shared_ptr<RenderPass> render_pass)
        {
            auto app_data = ApplicationData::data;

            // Create Frame Buffers
            {
                vk::FramebufferCreateInfo fb_info = {};
                fb_info.width 			= app_data->view_width;
                fb_info.height 			= app_data->view_height;
                fb_info.layers 			= 1;
                fb_info.renderPass 		= render_pass->getRenderPass();
                fb_info.attachmentCount = static_cast<uint32_t>(img_attachments.size());
                fb_info.pAttachments 	= img_attachments.data();
                fb_info.pNext 			= nullptr;

                frame_buffer_ = app_data->device.createFramebuffer(fb_info);
            }
        }

        FrameBuffer::~FrameBuffer()
        {
            vkDestroyFramebuffer(ApplicationData::data->device, frame_buffer_, nullptr);
        }

        vk::Framebuffer FrameBuffer::getFrameBufferKHR() const
        {
            return frame_buffer_;
        }
    }
}

