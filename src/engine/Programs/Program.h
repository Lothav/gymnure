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
        class Program
        {

        private:

            struct ProgramData
            {
                std::vector<std::unique_ptr<Descriptors::Texture>> textures       = {};
                std::unique_ptr<Vertex::VertexBuffer>              vertex_buffer  = nullptr;
                vk::DescriptorSet                                  descriptor_set = {}; // Each object must have a different DS
            };

            std::unique_ptr<Descriptors::Camera>                camera_          = nullptr;
            glm::mat4*                                          models_          = nullptr;
            std::unique_ptr<Memory::Buffer<glm::mat4>>          buffer_          = nullptr;

        public:

            std::vector<std::unique_ptr<ProgramData>>           data             = {};
            std::unique_ptr<Descriptors::DescriptorSet>         descriptor_set   = nullptr;
            std::unique_ptr<GraphicsPipeline::GraphicsPipeline> graphic_pipeline = nullptr;

            Program()
            {
                auto app_data = ApplicationData::data;

                //  Create Camera
                camera_ = std::make_unique<Descriptors::Camera>();
                camera_->initModelView(app_data->view_width, app_data->view_height);
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
                    program_data->vertex_buffer->createPrimitiveTriangle();

                data.push_back(std::move(program_data));
            }

            void prepare()
            {
                auto app_data = ApplicationData::data;

                size_t dynamicAlignment = Memory::Memory::getDynamicAlignment();
                size_t bufferSize = data.size() * dynamicAlignment;
                models_ = (glm::mat4 *) Memory::Memory::alignedAlloc(bufferSize, dynamicAlignment);

                for (int i = 0 ; i < data.size(); i ++)
                    models_[i] = glm::mat4(1.0f);

                models_[1] = glm::translate(models_[1], glm::vec3(2, 2, 2));

                struct BufferData buffer_data = {};
                buffer_data.usage      = vk::BufferUsageFlagBits::eUniformBuffer;
                buffer_data.properties = vk::MemoryPropertyFlagBits::eHostVisible;
                buffer_data.count      = bufferSize / sizeof(glm::mat4);

                buffer_ = std::make_unique<Memory::Buffer<glm::mat4>>(buffer_data);
                buffer_->updateBuffer(models_);

                std::vector<vk::WriteDescriptorSet, mem::StdAllocator<vk::WriteDescriptorSet>> writes = {};

                // Create program Descriptor Set.
                auto descriptors_sets = descriptor_set->createDescriptorSets(static_cast<uint32_t>(data.size()), 1, 0);

                vk::DescriptorBufferInfo *buffer_info = new vk::DescriptorBufferInfo();
                buffer_info->offset = 0;
                buffer_info->range  = VK_WHOLE_SIZE;
                buffer_info->buffer = buffer_->getBuffer();

                for (uint32_t i = 0; i < data.size(); i++)
                {
                    data[i]->descriptor_set = descriptors_sets[i];

                    vk::WriteDescriptorSet model_bind = {};
                    model_bind.pNext 			= nullptr;
                    model_bind.dstSet 			= data[i]->descriptor_set;
                    model_bind.descriptorCount 	= 1;
                    model_bind.descriptorType 	= vk::DescriptorType::eUniformBufferDynamic;
                    model_bind.pBufferInfo 		= buffer_info;
                    model_bind.dstBinding 		= 0;
                    writes.push_back(model_bind);

                    auto camera_bind = camera_->getWrite(data[i]->descriptor_set, 1);
                    writes.push_back(camera_bind);

                    for (uint32_t j = 0; j < data[i]->textures.size(); ++j)
                    {
                        auto texture_bind = data[i]->textures[j]->getWrite(data[i]->descriptor_set, j + 2);
                        writes.push_back(texture_bind);
                    }
                }

                app_data->device.updateDescriptorSets(writes, {});
            }
        };
    }
}

#endif //GYMNURE_PROGRAM_H
