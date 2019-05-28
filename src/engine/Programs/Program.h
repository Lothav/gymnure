//
// Created by luiz0tavio on 8/12/18.
//

#ifndef GYMNURE_PROGRAM_H
#define GYMNURE_PROGRAM_H

#include <Descriptors/Layout.h>
#include <Vertex/VertexBuffer.h>
#include <ApplicationData.hpp>
#include <GraphicsPipeline/GraphicsPipeline.h>
#include "ModelBuffer.hpp"

struct GymnureObjData
{
    std::string              obj_path       = "";
    std::string              obj_mtl        = "";
    std::vector<std::string> paths_textures = {};
};

namespace Engine
{
    namespace Programs
    {
        enum VertexInputType
        {
            POSITION = 1 << 1,
            NORMAL   = 1 << 2,
            UV       = 1 << 3
        };

        struct ProgramParams
        {
            vk::RenderPass          render_pass{};
            uint32_t                vi_types_mask = 0;
            Descriptors::LayoutData layout_data{};
            const std::string&      shaders_name;
        };

        class Program
        {

        private:

            struct ObjectData
            {
                std::vector<std::shared_ptr<Descriptors::Texture>> textures         = {};
                std::shared_ptr<Vertex::VertexBuffer>              vertex_buffer    = nullptr;
                vk::DescriptorSet                                  descriptor_set   = {}; // Each object must have a different DS
            };

            struct ProgramData
            {
                std::vector<std::shared_ptr<ObjectData>>            objects_data        = {};
                std::shared_ptr<Descriptors::Layout>                descriptor_layout   = nullptr;
                std::shared_ptr<GraphicsPipeline::GraphicsPipeline> graphic_pipeline    = nullptr;
                std::shared_ptr<ModelBuffer>                        model_buffer_       = nullptr;
            };

            std::vector<std::shared_ptr<ProgramData>> programs_data = {};

        public:

            explicit Program(const std::vector<ProgramParams>& programs)
            {
                for (const auto &program : programs)
                {
                    std::shared_ptr<ProgramData> program_data = std::make_shared<ProgramData>();

                    {
                        program_data->descriptor_layout = std::make_shared<Descriptors::Layout>(program.layout_data);
                    }
                    {
                        auto vert = Engine::GraphicsPipeline::Shader{};
                        vert.type = vk::ShaderStageFlagBits::eVertex;
                        vert.path = program.shaders_name + ".vert.spv";

                        auto frag = Engine::GraphicsPipeline::Shader{};
                        frag.type = vk::ShaderStageFlagBits::eFragment;
                        frag.path = program.shaders_name + ".frag.spv";

                        std::vector<Engine::GraphicsPipeline::Shader> shaders = {vert, frag};
                        program_data->graphic_pipeline = std::make_shared<GraphicsPipeline::GraphicsPipeline>(std::move(shaders));
                    }
                    {
                        std::vector<vk::VertexInputAttributeDescription> vi_attribs = {};
                        vk::VertexInputAttributeDescription vi_attrib{};
                        vi_attrib.binding 	= 0;

                        uint32_t location = 0;
                        if(program.vi_types_mask & VertexInputType::POSITION)
                        {
                            vi_attrib.location 	= location++;
                            vi_attrib.format 	= vk::Format::eR32G32B32Sfloat;
                            vi_attrib.offset 	= static_cast<uint32_t>(offsetof(VertexData, pos));

                            vi_attribs.push_back(vi_attrib);
                        }

                        if(program.vi_types_mask & VertexInputType::UV)
                        {
                            vi_attrib.location 	= location++;
                            vi_attrib.format 	= vk::Format::eR32G32Sfloat;
                            vi_attrib.offset 	= static_cast<uint32_t>(offsetof(VertexData, uv));

                            vi_attribs.push_back(vi_attrib);
                        }

                        if(program.vi_types_mask & VertexInputType::NORMAL)
                        {
                            vi_attrib.location 	= location++;
                            vi_attrib.format 	= vk::Format::eR32G32B32Sfloat;
                            vi_attrib.offset 	= static_cast<uint32_t>(offsetof(VertexData, normal));

                            vi_attribs.push_back(vi_attrib);
                        }

                        program_data->graphic_pipeline->addViAttributes(vi_attribs);
                    }

                    vk::PipelineLayout pl = program_data->descriptor_layout->getPipelineLayout();
                    program_data->graphic_pipeline->create(pl, program.render_pass, vk::CullModeFlagBits::eBack);

                    programs_data.push_back(std::move(program_data));
                }
            }

            void addObjData(GymnureObjData&& obj_data, uint32_t program_index)
            {
                auto app_data = ApplicationData::data;
                auto object_data = std::make_shared<ObjectData>();

                // Load Textures
                for (std::string& texture_path : obj_data.paths_textures)
                    if(!texture_path.empty())
                        object_data->textures.push_back(std::make_shared<Descriptors::Texture>(texture_path));

                // Load Vertex
                object_data->vertex_buffer = std::make_shared<Vertex::VertexBuffer>();
                if(!obj_data.obj_path.empty())
                    // Load obj using TinyObjLoader.
                    object_data->vertex_buffer->loadObjModelVertices(obj_data.obj_path, obj_data.obj_mtl);
                else
                    // Empty obj_path. Use triangle as default vertex data.
                    object_data->vertex_buffer->createPrimitiveQuad();

                programs_data[program_index]->objects_data.push_back(std::move(object_data));
            }

            virtual void prepare(const std::shared_ptr<Descriptors::Camera>& camera)
            {
                auto app_data = ApplicationData::data;

                for(auto& program_data : programs_data)
                {
                    auto data_size = static_cast<uint32_t>(program_data->objects_data.size());

                    program_data->model_buffer_ = std::make_shared<ModelBuffer>(data_size);

                    std::vector<vk::WriteDescriptorSet> writes = {};

                    // Create program Descriptor Set.
                    auto descriptors_sets = program_data->descriptor_layout->createDescriptorSets(data_size);

                    std::shared_ptr<Descriptors::LayoutData> layout_data = program_data->descriptor_layout->getLayoutData();

                    for (uint32_t i = 0; i < data_size; i++)
                    {
                        program_data->objects_data[i]->descriptor_set = descriptors_sets[i];

                        if(layout_data->has_model_matrix)
                        {
                            auto model_bind = program_data->model_buffer_->getWrite(descriptors_sets[i], 0);
                            writes.push_back(model_bind);
                        }

                        if(layout_data->has_view_projection_matrix)
                        {
                            auto camera_binds = camera->getWrites(descriptors_sets[i], 1, 2);
                            writes.push_back(camera_binds[0]);
                            writes.push_back(camera_binds[1]);
                        }

                        if(layout_data->fragment_texture_count > 0)
                        {
                            auto texture_bind = program_data->objects_data[i]->textures[0]->getWrite(descriptors_sets[i], 3);
                            writes.push_back(texture_bind);
                        }
                    }

                    app_data->device.updateDescriptorSets(writes, {});
                }
            }

            std::vector<std::shared_ptr<ProgramData>> getProgramsData() const
            {
                return programs_data;
            }
        };
    }
}

#endif //GYMNURE_PROGRAM_H
