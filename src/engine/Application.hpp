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

        static SyncPrimitives::SyncPrimitives* 		    sync_primitives;
        static CommandBuffers*                          command_buffer;
        static std::vector<Programs::Program*>          programs;
        static RenderPass::FrameBuffer* 				frame_buffer;

        static uint32_t 								current_buffer_;

    public:

        static void create(const std::vector<const char *>& instance_extension_names);
        static void setupSurface(const uint32_t& width, const uint32_t& height);
        static void prepare();
        static void draw();

        static void destroy();

        static uint createPhongProgram();
        static uint createSkyboxProgram();
        static uint createDefaultProgram();
        static void addObjData(uint, GymnureObjData&&);
        static std::vector<Programs::Program*> getPrograms()
        {
            return programs;
        }
    };

}

#endif //GYMNURE_APPLICATION_HPP
