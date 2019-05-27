#ifndef GYMNURE_APPLICATION_HPP
#define GYMNURE_APPLICATION_HPP

#include <SyncPrimitives/SyncPrimitives.h>
#include <RenderPass/RenderPass.h>
#include <RenderPass/FrameBuffer.h>
#include <GraphicsPipeline/Forward.hpp>
#include "CommandBuffer.h"

#define APP_NAME "Gymnure"

namespace Engine
{
    class Application
    {
    private:

        static std::vector<std::shared_ptr<Programs::Program>>          programs;
        static std::shared_ptr<Descriptors::Camera>                     main_camera;

        static std::unique_ptr<GraphicsPipeline::Forward>               forward;

    public:

        static void create(const std::vector<const char *>& instance_extension_names);
        static void setupSurface(const uint32_t& width, const uint32_t& height);
        static void setupPipelines();

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
