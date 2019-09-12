#ifndef GYMNURE_COMMANDBUFFER_H
#define GYMNURE_COMMANDBUFFER_H

#include <memory>
#include <vector>
#include <cassert>
#include <GraphicsPipeline/GraphicsPipeline.h>
#include <Programs/Program.h>
#include "Descriptors/Layout.h"
#include "SyncPrimitives/SyncPrimitives.h"
#include "Vertex/VertexBuffer.h"
#include <RenderPass/RenderPass.h>

namespace Engine
{
    class CommandBuffer
    {

    private:

        vk::CommandBuffer command_buffer_{};

    public:

        CommandBuffer();
        ~CommandBuffer();

        void bindGraphicCommandBuffer(
            std::vector<vk::ClearValue> clear_values,
            const std::shared_ptr<RenderPass::RenderPass>& render_pass,
            const std::shared_ptr<RenderPass::FrameBuffer>& frame_buffer,
            const std::vector<std::shared_ptr<Programs::Program>>& programs);

        vk::CommandBuffer getCommandBuffer() const;

    };
}

#endif //GYMNURE_COMMANDBUFFER_H
