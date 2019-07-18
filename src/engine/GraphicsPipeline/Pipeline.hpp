#ifndef GYMNURE_PIPELINE_HPP
#define GYMNURE_PIPELINE_HPP

#include <vulkan/vulkan.hpp>
#include <Memory/BufferImage.h>
#include <RenderPass/RenderPass.h>
#include <Memory/ImageFormats.hpp>
#include <RenderPass/FrameBuffer.h>
#include <SyncPrimitives/SyncPrimitives.h>
#include <CommandBuffer.h>

namespace Engine
{
    namespace GraphicsPipeline
    {
        class Pipeline
        {

        private:

            std::vector<std::unique_ptr<Descriptors::Texture>>      render_textures_        = {};
            std::unique_ptr<Memory::BufferImage> 					depth_buffer_ 	        = nullptr;

            std::shared_ptr<RenderPass::RenderPass> 				render_pass_ 	        = nullptr;
            std::unique_ptr<SyncPrimitives::SyncPrimitives> 	    sync_primitives_        = nullptr;
            std::vector<std::shared_ptr<RenderPass::FrameBuffer>> 	frame_buffers_ 	        = {};
            std::vector<std::unique_ptr<CommandBuffer>>             command_buffers_        = {};

            uint32_t 												current_buffer_         = 0;
            uint32_t                                                color_targets_count_    = 0;
            bool                                                    present_                = false;

        public:

            explicit Pipeline(bool has_depth = true);
            // @TODO IMPLEMENT HAS_DEPTH
            Pipeline(uint32_t color_targets_count, bool has_depth = true);

            vk::RenderPass getRenderPass() const;
            void prepare(const std::vector<std::shared_ptr<Programs::Program>>& programs);
            void render();
        };
    }
}

#endif //GYMNURE_PIPELINE_HPP
