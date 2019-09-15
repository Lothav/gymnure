#ifndef GYMNURE_PROGRAM_H
#define GYMNURE_PROGRAM_H

#include <imgui/imgui.h>
#include <Descriptors/Camera.h>
#include <Descriptors/Layout.h>
#include <ModelBuffer.hpp>
#include "Vertex/VertexBuffer.h"

struct GymnureObjData
{
    std::string              obj_path       = "";
    std::string              obj_mtl        = "";
    std::vector<std::string> paths_textures = {};
    std::vector<std::shared_ptr<Engine::Descriptors::Texture>> textures = {};
};

enum GymnureObjDataType
{
    OBJ,
    FBX
};

namespace Engine::Programs
{
    enum VertexInputType
    {
        NONE     = 1 << 0,
        POSITION = 1 << 1,
        NORMAL   = 1 << 2,
        UV       = 1 << 3,
        COLOR    = 1 << 4,
    };

    struct ProgramParams {
        uint32_t vi_types_mask = 0;
        Descriptors::LayoutData layout_data{};
        std::string shaders_name;
    };

    class Program {

    private:

        struct ObjectData
        {
            std::vector<std::shared_ptr<Descriptors::Texture>> textures = {};
            std::shared_ptr<Vertex::VertexBuffer<VertexData, uint32_t>> vertex_buffer = nullptr;
            vk::DescriptorSet descriptor_set = {}; // Each object must have a different DS
        };

        struct UiData
        {
            std::vector<std::shared_ptr<Descriptors::Texture>> textures = {};
            std::shared_ptr<Vertex::VertexBuffer<ImDrawVert, ImDrawIdx>> vertex_buffer = nullptr;
            vk::DescriptorSet descriptor_set = {}; // Each object must have a different DS
        };

        struct ProgramData
        {
            std::vector<std::shared_ptr<ObjectData>> objects_data = {};
            std::vector<std::shared_ptr<UiData>> ui_data = {};
            std::shared_ptr<Descriptors::Layout> descriptor_layout = nullptr;
            std::shared_ptr<GraphicsPipeline::GraphicsPipeline> graphic_pipeline = nullptr;
            std::shared_ptr<ModelBuffer> model_buffer_ = nullptr;
        };

        std::shared_ptr<ProgramData> program_data_ = std::make_shared<ProgramData>();

    public:

        explicit Program(const ProgramParams &p_config, vk::RenderPass render_pass);

        void addUiData(const std::vector<ImDrawVert>& vertexData, const std::vector<ImDrawIdx>& indexBuffer);
        void addObjData(GymnureObjData &&obj_data, const GymnureObjDataType& data_type);

        void prepare(const std::shared_ptr<Descriptors::Camera> &camera);
        [[nodiscard]] std::shared_ptr<ProgramData> getProgramsData() const;
    };
}

#endif //GYMNURE_PROGRAM_H
