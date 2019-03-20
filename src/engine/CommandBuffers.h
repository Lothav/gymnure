//
// Created by luizorv on 9/2/17.
//

#ifndef OBSIDIAN2D_COMMANDBUFFERS_H
#define OBSIDIAN2D_COMMANDBUFFERS_H

#include <vector>
#include <cassert>
#include <GraphicPipeline/GraphicPipeline.h>
#include <Programs/Program.h>
#include "RenderPass/RenderPass.h"
#include "Descriptors/DescriptorSet.h"
#include "SyncPrimitives/SyncPrimitives.h"
#include "Vertex/VertexBuffer.h"
#include <memory>

namespace Engine
{
    class CommandBuffers
    {

    private:

        std::vector<vk::CommandBuffer> command_buffers_ = {};

    public:

        explicit CommandBuffers(uint swapchain_images_count);

        ~CommandBuffers();

        void* operator new(std::size_t size)
        {
            return mem::Provider::getMemory(size);
        }

        void operator delete(void* ptr)
        {
            // Do not free memory here!
        }

        void bindGraphicCommandBuffer(std::vector<Programs::Program*> programs, RenderPass::RenderPass* render_pass, uint32_t width, uint32_t height);

        std::vector<vk::CommandBuffer> getCommandBuffers();

    };
}

#endif //OBSIDIAN2D_COMMANDBUFFERS_H
