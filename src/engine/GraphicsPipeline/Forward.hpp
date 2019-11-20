#ifndef GYMNURE_FORWARD_HPP
#define GYMNURE_FORWARD_HPP

#include <GraphicsPipeline/Pipeline.hpp>

namespace Engine::GraphicsPipeline
{
    class Forward
    {

    private:

        std::unique_ptr<Pipeline>                           pipeline_ = {};
        std::vector<std::shared_ptr<Programs::Program>>     programs_ = {};

    public:

        Forward();

        uint32_t createProgram(Programs::ProgramParams &&params);
        void addObjData(uint32_t program_id, GymnureObjData&& data, const GymnureObjDataType& type);
        void addUiData(uint32_t program_id, const std::vector<ImDrawVert>& vertexData, const std::vector<ImDrawIdx>& indexBuffer);
        void prepare(const std::shared_ptr<Descriptors::Camera> &camera);
        void render();
    };
}

#endif //GYMNURE_FORWARD_HPP
