#ifndef GYMNURE_APPLICATION_HPP
#define GYMNURE_APPLICATION_HPP

#include <GraphicsPipeline/Forward.hpp>
#include <GraphicsPipeline/Deferred.hpp>

#define APP_NAME "Gymnure"

namespace Engine
{
    enum ProgramPipeline
    {
        FORWARD,
        DEFERRED
    };

    class Application
    {
    private:

        static std::shared_ptr<Descriptors::Camera>                     main_camera;

        static std::unique_ptr<GraphicsPipeline::Forward>               forward_pipeline_;
        static std::unique_ptr<GraphicsPipeline::Deferred>              deferred_pipeline_;
        static std::vector<ProgramPipeline>                             programs_;

    public:

        static void create(const std::vector<const char *>& instance_extension_names);
        static void setupSurface(const uint32_t& width, const uint32_t& height);

        static void prepare();
        static void draw();
        static void destroy();

        static std::shared_ptr<Descriptors::Camera> getMainCamera();
        static uint32_t createPhongProgram();
        static uint32_t createDeferredProgram();
        static uint32_t createInterfaceProgram();

        static void addObjData(uint32_t, GymnureObjData&&, const GymnureObjDataType& type);
        static void addUiData(uint32_t program_id, const std::vector<ImDrawVert>& vertexData, const std::vector<ImDrawIdx>& indexBuffer);
    };

}

#endif //GYMNURE_APPLICATION_HPP
