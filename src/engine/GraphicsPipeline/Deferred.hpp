
#ifndef GYMNURE_DEFERRED_HPP
#define GYMNURE_DEFERRED_HPP

#include <memory>
#include <RenderPass/RenderPass.h>
#include <Memory/ImageFormats.hpp>
#include <RenderPass/FrameBuffer.h>
#include "Memory/BufferImage.h"

namespace Engine
{
    namespace GraphicsPipeline
    {
        class Deferred
        {

        private:

            struct {
                std::unique_ptr<Memory::BufferImage> albedo;
            } g_buffer_;

            std::vector<std::shared_ptr<RenderPass::FrameBuffer>>	frame_buffers_;
            std::shared_ptr<RenderPass::RenderPass>                 render_pass_;

        public:

            Deferred()
            {
                auto app_data = ApplicationData::data;

                {
                    Memory::ImageProps img_props = {};
                    img_props.width             = static_cast<uint32_t>(app_data->view_width);
                    img_props.height            = static_cast<uint32_t>(app_data->view_height);
                    img_props.format            = vk::Format::eR8G8B8A8Unorm;
                    img_props.usage             =                 // Image will be
                        vk::ImageUsageFlagBits::eSampled |        // sampled in shaders and
                        vk::ImageUsageFlagBits::eColorAttachment; // used as Framebuffer attachment
                    img_props.tiling            = vk::ImageTiling::eOptimal;
                    img_props.image_props_flags = vk::MemoryPropertyFlagBits::eDeviceLocal;

                    g_buffer_.albedo = std::make_unique<Memory::BufferImage>(img_props);
                }

                // Create Render Pass
                {
                    std::vector<RenderPass::RpAttachments> rp_attachments = {};
                    render_pass_ = std::make_shared<RenderPass::RenderPass>(rp_attachments);
                }

                {
                    std::vector<vk::ImageView> img_attachments = { g_buffer_.albedo->view };
                    frame_buffers_.push_back(std::make_shared<RenderPass::FrameBuffer>(img_attachments, render_pass_));
                }
            }

            ~Deferred()
            {
                render_pass_.reset();
            }
        };
    }
}
#endif //GYMNURE_DEFERRED_HPP
