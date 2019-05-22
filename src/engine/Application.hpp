#ifndef GYMNURE_APPLICATION_HPP
#define GYMNURE_APPLICATION_HPP

#include <SyncPrimitives/SyncPrimitives.h>
#include <RenderPass/RenderPass.h>
#include <RenderPass/FrameBuffer.h>
#include "CommandBuffers.h"

#define APP_NAME "Gymnure"

namespace Engine
{
    class Application
    {
    private:

        static std::vector<std::shared_ptr<Programs::Program>>          programs;
        static std::unique_ptr<SyncPrimitives::SyncPrimitives> 	        sync_primitives;
        static std::vector<std::shared_ptr<RenderPass::FrameBuffer>>	frame_buffers;
        static std::unique_ptr<CommandBuffers>                          command_buffer;
        static std::shared_ptr<Descriptors::Camera>                     main_camera;
        static std::shared_ptr<RenderPass::SwapChain> 			        swap_chain;
        static std::shared_ptr<RenderPass::RenderPass>                  render_pass_;
        static std::shared_ptr<Memory::BufferImage>                     depth_buffer_;

        static uint32_t 								                current_buffer_;

    public:

        static void create(const std::vector<const char *>& instance_extension_names);
        static void setupSurface(const uint32_t& width, const uint32_t& height);
        static void prepare();
        static void draw();
        static void destroy();

        static std::shared_ptr<Descriptors::Camera> getMainCamera();
        static uint createDefaultProgram();
        static uint createPhongProgram();
        static void addObjData(uint, GymnureObjData&&);
    };

}

#endif //GYMNURE_APPLICATION_HPP
