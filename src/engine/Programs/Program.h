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
        };

        class Program
        {

        protected:

            vk::Queue                                   transfer_queue_{};
            std::unique_ptr<Descriptors::UniformBuffer> uniform_buffer_ = nullptr;

        public:

            Descriptors::DescriptorSet*                 descriptor_set   = nullptr;
            std::vector<std::unique_ptr<ProgramData>>   data             = {};
            GraphicsPipeline::GraphicsPipeline*         graphic_pipeline = nullptr;

            ~Program()
            {
                auto device = Engine::ApplicationData::data->device;

                delete graphic_pipeline;
                delete descriptor_set;
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

            virtual void init(vk::RenderPass render_pass) = 0;

            virtual void addObjData(GymnureObjData&& obj_data) = 0;

            virtual void prepare() const = 0;
        };
    }
}

#endif //GYMNURE_PROGRAM_H
