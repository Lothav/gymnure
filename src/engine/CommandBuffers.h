//
// Created by luizorv on 9/2/17.
//

#ifndef OBSIDIAN2D_COMMANDBUFFERS_H
#define OBSIDIAN2D_COMMANDBUFFERS_H

#include <memory>
#include <vector>
#include <cassert>
#include <GraphicsPipeline/GraphicsPipeline.h>
#include <Programs/Program.h>
#include "Descriptors/DescriptorSet.h"
#include "SyncPrimitives/SyncPrimitives.h"
#include "Vertex/VertexBuffer.h"
#include <RenderPass/RenderPass.h>

namespace Engine
{
    class CommandBuffers
    {

    private:

        std::vector<vk::CommandBuffer> command_buffers_ = {};

    public:

        explicit CommandBuffers(uint swapchain_images_count);

        ~CommandBuffers();

        void bindGraphicCommandBuffer(
            uint32_t image_count,
            std::shared_ptr<RenderPass::RenderPass> render_pass,
            std::vector<std::shared_ptr<Programs::Program>> programs,
            std::vector<std::shared_ptr<RenderPass::FrameBuffer>> frame_buffer);

        vk::CommandBuffer getCommandBuffer(uint32_t index) const;

    };
}

#endif //OBSIDIAN2D_COMMANDBUFFERS_H
