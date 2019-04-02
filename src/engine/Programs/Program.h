//
// Created by luiz0tavio on 8/12/18.
//

#ifndef GYMNURE_PROGRAM_H
#define GYMNURE_PROGRAM_H

#include <Descriptors/DescriptorSet.h>
#include <Vertex/VertexBuffer.h>
#include <memancpp/Provider.hpp>
#include <ApplicationData.hpp>
#include <GraphicsPipeline/GraphicsPipeline.h>

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
        struct ProgramData
        {
            std::vector<std::unique_ptr<Descriptors::Texture>>  textures        = {};
            std::unique_ptr<Vertex::VertexBuffer>               vertex_buffer   = nullptr;
            vk::DescriptorSet                                   descriptor_set  = {};
            glm::mat4                                           model_matrix    = glm::mat4(1.0f);
        };

        class Program
        {

        private:

            std::unique_ptr<Descriptors::UniformBuffer>         uniform_buffer_ = nullptr;

        public:

            std::unique_ptr<Descriptors::DescriptorSet>         descriptor_set   = nullptr;
            std::vector<std::unique_ptr<ProgramData>>           data             = {};
            std::unique_ptr<GraphicsPipeline::GraphicsPipeline> graphic_pipeline = nullptr;

            Program()
            {
                auto app_data = ApplicationData::data;

                //  Create Uniform Buffer
                uniform_buffer_ = std::make_unique<Descriptors::UniformBuffer>();
                uniform_buffer_->initModelView(app_data->view_width, app_data->view_height);
            }

            ~Program()
            {
                auto device = Engine::ApplicationData::data->device;

                graphic_pipeline.reset();
                descriptor_set.reset();
                data.clear();
            }

            void* operator new (std::size_t size)
            {
                return mem::Provider::getMemory(size);
            }

            void operator delete(void* ptr)
            {
                // Do not free memory here!
            }

            virtual void addObjData(GymnureObjData&& obj_data)
            {
                auto app_data = ApplicationData::data;
                auto program_data = std::make_unique<ProgramData>();

                // Load Textures
                for (std::string& texture_path : obj_data.paths_textures) {
                    if(!texture_path.empty())
                        program_data->textures.push_back(
                            std::make_unique<Descriptors::Texture>(texture_path)) ;
                }

                // Load Vertex
                program_data->vertex_buffer = std::make_unique<Vertex::VertexBuffer>();
                if(!obj_data.obj_path.empty())
                    // Load obj using TinyObjLoader.
                    program_data->vertex_buffer->loadObjModelVertices(obj_data.obj_path, obj_data.obj_mtl);
                else
                    // Empty obj_path. Use triangle as default vertex data.
                    program_data->vertex_buffer->createPrimitiveTriangle();

                data.push_back(std::move(program_data));
            }

            virtual void prepare() const
            {
                auto app_data = ApplicationData::data;

                std::vector<vk::WriteDescriptorSet, mem::StdAllocator<vk::WriteDescriptorSet>> writes = {};

                // Create program Descriptor Set.
                auto descriptors_sets = descriptor_set->createDescriptorSets(static_cast<uint32_t>(data.size()), 1, 1);

                for (uint32_t i = 0; i < data.size(); i++)
                {
                    data[i]->descriptor_set = descriptors_sets[i];

                    uint32_t bind_count = 0;
                    for (uint32_t j = 0; j < data[i]->textures.size(); ++j)
                    {
                        auto texture_bind = data[i]->textures[j]->getWrite(data[i]->descriptor_set, bind_count++);
                        writes.push_back(texture_bind);
                    }

                    auto uniform_bind = uniform_buffer_->getWrite(data[i]->descriptor_set, bind_count);
                    writes.push_back(uniform_bind);
                }

                app_data->device.updateDescriptorSets(writes, {});
            }
        };
    }
}

#endif //GYMNURE_PROGRAM_H
