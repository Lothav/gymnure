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
        class Program
        {

        private:

            struct ProgramData
            {
                std::vector<std::unique_ptr<Descriptors::Texture>> textures       = {};
                std::unique_ptr<Vertex::VertexBuffer>              vertex_buffer  = nullptr;
                vk::DescriptorSet                                  descriptor_set = {}; // Each object must have a different DS
            };

        protected:

            std::unique_ptr<ModelBuffer>                        model_buffer_    = nullptr;

        public:

            std::vector<std::unique_ptr<ProgramData>>           data             = {};
            std::unique_ptr<Descriptors::DescriptorSet>         descriptor_set   = nullptr;
            std::unique_ptr<GraphicsPipeline::GraphicsPipeline> graphic_pipeline = nullptr;

            Program() = default;

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

            void addObjData(GymnureObjData&& obj_data)
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
                    program_data->vertex_buffer->createPrimitiveQuad();

                data.push_back(std::move(program_data));
            }

            virtual void prepare(const std::shared_ptr<Descriptors::Camera>& camera)
            {
                auto app_data = ApplicationData::data;

                model_buffer_ = std::make_unique<ModelBuffer>(data.size());

                std::vector<vk::WriteDescriptorSet, mem::StdAllocator<vk::WriteDescriptorSet>> writes = {};

                // Create program Descriptor Set.
                auto descriptors_sets = descriptor_set->createDescriptorSets(static_cast<uint32_t>(data.size()), 1, 0);

                for (uint32_t i = 0; i < data.size(); i++)
                {
                    data[i]->descriptor_set = descriptors_sets[i];

                    auto model_bind = model_buffer_->getWrite(descriptors_sets[i], 0);
                    writes.push_back(model_bind);

                    //auto camera_bind = camera->getWrite(descriptors_sets[i], 1);
                    //writes.push_back(camera_bind);

                    for (uint32_t j = 0; j < data[i]->textures.size(); ++j)
                    {
                        auto texture_bind = data[i]->textures[j]->getWrite(descriptors_sets[i], j + 2);
                        writes.push_back(texture_bind);
                    }
                }

                app_data->device.updateDescriptorSets(writes, {});
            }
        };
    }
}

#endif //GYMNURE_PROGRAM_H
