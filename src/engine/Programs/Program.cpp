#include <GraphicsPipeline/GraphicsPipeline.h>
#include <Util/ModelDataLoader.h>
#include "Program.h"

namespace Engine::Programs
{
    Program::Program(const ProgramParams &p_config, vk::RenderPass render_pass)
    {
        program_data_->descriptor_layout = std::make_shared<Descriptors::Layout>(p_config.layout_data);

        auto vert = Engine::GraphicsPipeline::Shader{};
        vert.type = vk::ShaderStageFlagBits::eVertex;
        vert.path = p_config.shaders_name + "_vs.spv";

        auto frag = Engine::GraphicsPipeline::Shader{};
        frag.type = vk::ShaderStageFlagBits::eFragment;
        frag.path = p_config.shaders_name + "_fs.spv";

        std::vector<Engine::GraphicsPipeline::Shader> shaders = {vert, frag};
        program_data_->graphic_pipeline = std::make_shared<GraphicsPipeline::GraphicsPipeline>(std::move(shaders));

        std::vector<vk::VertexInputAttributeDescription> vi_attribs = {};
        vk::VertexInputAttributeDescription vi_attrib{};
        vi_attrib.binding = 0;

        uint32_t location = 0;
        if (p_config.vi_types_mask & VertexInputType::POSITION)
        {
            vi_attrib.location = location++;
            vi_attrib.format = vk::Format::eR32G32B32Sfloat;
            vi_attrib.offset = static_cast<uint32_t>(offsetof(VertexData, pos));

            vi_attribs.push_back(vi_attrib);
        }

        if (p_config.vi_types_mask & VertexInputType::UV)
        {
            vi_attrib.location = location++;
            vi_attrib.format = vk::Format::eR32G32Sfloat;
            vi_attrib.offset = static_cast<uint32_t>(offsetof(VertexData, uv));

            vi_attribs.push_back(vi_attrib);
        }

        if (p_config.vi_types_mask & VertexInputType::NORMAL)
        {
            vi_attrib.location = location++;
            vi_attrib.format = vk::Format::eR32G32B32Sfloat;
            vi_attrib.offset = static_cast<uint32_t>(offsetof(VertexData, normal));

            vi_attribs.push_back(vi_attrib);
        }

        if (p_config.vi_types_mask & VertexInputType::COLOR)
        {
            vi_attrib.location = location++;
            vi_attrib.format = vk::Format::eR8G8B8A8Unorm;
            vi_attrib.offset = static_cast<uint32_t>(offsetof(VertexData, color));

            vi_attribs.push_back(vi_attrib);
        }

        program_data_->graphic_pipeline->addViAttributes(vi_attribs);

        vk::PipelineLayout pl = program_data_->descriptor_layout->getPipelineLayout();
        program_data_->graphic_pipeline->create(pl, render_pass, vk::CullModeFlagBits::eBack);
    }

    void Program::addUiData(const std::vector<ImDrawVert>& vertexData, const std::vector<ImDrawIdx>& indexBuffer)
    {
        auto object_data = std::make_shared<UiData>();
        object_data->vertex_buffer = std::make_shared<Vertex::VertexBuffer<ImDrawVert, ImDrawIdx>>();
        object_data->vertex_buffer->initBuffers(vertexData, indexBuffer);

        program_data_->ui_data.push_back(std::move(object_data));
    }

    void Program::addObjData(GymnureObjData &&obj_data, const GymnureObjDataType& data_type)
    {
        if(data_type == GymnureObjDataType::OBJ)
        {
            auto app_data = ApplicationData::data;
            auto object_data = std::make_shared<ObjectData>();

            // Load Textures
            for (std::string &texture_path : obj_data.paths_textures)
                if (!texture_path.empty())
                    object_data->textures.push_back(std::make_shared<Descriptors::Texture>(texture_path));
            for (auto &texture : obj_data.textures)
                object_data->textures.push_back(std::move(texture));

            // Load Vertex
            object_data->vertex_buffer = std::make_shared<Vertex::VertexBuffer<VertexData, uint32_t>>();
            if (!obj_data.obj_path.empty())
            {
                std::unique_ptr<Model> model = Util::ModelDataLoader::LoadOBJData(obj_data.obj_path, obj_data.obj_mtl);
                for (const std::shared_ptr<Mesh>& mesh : *model->meshes)
                {
                    // @TODO support .obj with multiple meshes
                    object_data->vertex_buffer->initBuffers(*mesh->vertexData);
                }
            }
            else
                // Empty obj_path. Use triangle as default vertex data.
                object_data->vertex_buffer->createPrimitiveQuad();

            program_data_->objects_data.push_back(std::move(object_data));

            return;
        }
        else if(data_type == GymnureObjDataType::FBX)
        {
            std::unique_ptr<Model> model = Util::ModelDataLoader::LoadFBXData(obj_data.obj_path);

            for (const std::shared_ptr<Mesh>& mesh : *model->meshes)
            {
                auto object_data = std::make_shared<ObjectData>();
                object_data->vertex_buffer = std::make_shared<Vertex::VertexBuffer<VertexData, uint32_t>>();
                object_data->vertex_buffer->initBuffers(*mesh->vertexData);

                std::string texture_path = mesh->material->texture_path;
                if(!texture_path.empty())
                {
                    std::cout << texture_path << std::endl;
                    auto texture = std::make_shared<Descriptors::Texture>(texture_path);

                    object_data->textures.push_back(std::move(texture));
                }
                program_data_->objects_data.push_back(std::move(object_data));
            }

            return;
        }

        throw std::invalid_argument("data_type param not supported!");
    }

    void Program::prepare(const std::shared_ptr<Descriptors::Camera> &camera)
    {
        auto app_data = ApplicationData::data;
        auto data_size = static_cast<uint32_t>(program_data_->objects_data.size());

        if (data_size == 0)
            return;

        program_data_->model_buffer_ = std::make_shared<ModelBuffer>(data_size);

        std::vector<vk::WriteDescriptorSet> writes = {};

        // Create program Descriptor Set.
        auto descriptors_sets = program_data_->descriptor_layout->createDescriptorSets(data_size);

        std::shared_ptr<Descriptors::LayoutData> layout_data = program_data_->descriptor_layout->getLayoutData();

        for (uint32_t i = 0; i < data_size; i++)
        {
            program_data_->objects_data[i]->descriptor_set = descriptors_sets[i];

            if (layout_data->has_model_matrix) {
                auto model_bind = program_data_->model_buffer_->getWrite(descriptors_sets[i], 0);
                writes.push_back(model_bind);
            }

            if (layout_data->has_view_projection_matrix) {
                auto camera_binds = camera->getWrites(descriptors_sets[i], 1, 2);
                writes.push_back(camera_binds[0]);
                writes.push_back(camera_binds[1]);
            }

            if (layout_data->fragment_texture_count > 0) {
                auto texture_bind = program_data_->objects_data[i]->textures[0]->getWrite(
                    descriptors_sets[i], 3);
                writes.push_back(texture_bind);
            }
        }

        app_data->device.updateDescriptorSets(writes, {});
    }

    [[nodiscard]] std::shared_ptr<Program::ProgramData> Program::getProgramsData() const
    {
        return program_data_;
    }
}