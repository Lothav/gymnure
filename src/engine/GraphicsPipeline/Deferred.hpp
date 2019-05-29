
#ifndef GYMNURE_DEFERRED_HPP
#define GYMNURE_DEFERRED_HPP

#include <memory>
#include <RenderPass/RenderPass.h>
#include <Memory/ImageFormats.hpp>
#include <RenderPass/FrameBuffer.h>
#include <CommandBuffer.h>
#include "Memory/BufferImage.h"

namespace Engine
{
    namespace GraphicsPipeline
    {
        class Deferred
        {

        private:

            struct {
                std::unique_ptr<Descriptors::Texture> albedo = nullptr;
            } g_buffer_;

            std::vector<std::shared_ptr<RenderPass::FrameBuffer>>	frame_buffers_      = {};
            std::shared_ptr<RenderPass::RenderPass>                 render_pass_        = nullptr;
            std::vector<std::unique_ptr<CommandBuffer>>             command_buffers_    = {};

            std::vector<std::unique_ptr<Programs::Program>>         programs_           = {};

        public:

            Deferred();

            vk::RenderPass getRenderPass() const;
            void prepare(const std::vector<std::shared_ptr<Programs::Program>>& programs);
            void render();

            void createProgram()
            {

            }
        };
    }
}
#endif //GYMNURE_DEFERRED_HPP
