#ifndef GYMNURE_DEFERRED_HPP
#define GYMNURE_DEFERRED_HPP

#include <memory>
#include <RenderPass/RenderPass.h>
#include <Memory/ImageFormats.hpp>
#include <RenderPass/FrameBuffer.h>
#include <CommandBuffer.h>
#include "Memory/BufferImage.h"
#include "Pipeline.hpp"

namespace Engine
{
    namespace GraphicsPipeline
    {
        class Deferred
        {

        private:

            struct Passes
            {
                std::shared_ptr<Programs::Program> mrt;
                std::shared_ptr<Programs::Program> present;
            };

            struct {
                std::unique_ptr<Descriptors::Texture> albedo = nullptr;
            } g_buffer_;

            std::vector<std::unique_ptr<CommandBuffer>>             command_buffers_ = {};

            std::unique_ptr<Pipeline>                               pipeline_ = {};
            std::vector<Passes>                                     programs_ = {};
            uint32_t                                                object_count_ = 0;

        public:

            Deferred();

            uint32_t createProgram(Programs::ProgramParams &&mrt, Programs::ProgramParams &&present);
            void addObjData(uint32_t program_id, GymnureObjData&& data, const GymnureObjDataType& type);
            void prepare(const std::shared_ptr<Descriptors::Camera> &camera);
            void render();
        };
    }
}
#endif //GYMNURE_DEFERRED_HPP
