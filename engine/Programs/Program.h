//
// Created by luiz0tavio on 8/12/18.
//

#ifndef GYMNURE_PROGRAM_H
#define GYMNURE_PROGRAM_H

#include <GraphicPipeline/GraphicPipeline.h>
#include <Descriptors/DescriptorSet.h>
#include <Vertex/VertexBuffer.h>

struct GymnureObjData {
    std::string             path_obj        = "";
    std::string             path_texture    = "";
    std::vector<VertexData> vertex_data     = {};
    char*                   obj_mtl         = nullptr;
};

namespace Engine
{
    namespace Programs
    {
        struct ProgramData {
            Descriptors::Texture  texture         = {};
            Vertex::VertexBuffer* vertex_buffer   = nullptr;
            VkDescriptorPool      descriptor_pool = nullptr;
            VkDescriptorSet       descriptor_set  = nullptr;
        };

        class Program
        {

        protected:

            struct DescriptorSetParams ds_params_ = {};

        public:

            Descriptors::DescriptorSet*         descriptor_layout   = nullptr;
            std::vector<ProgramData*>           data                = {};
            GraphicPipeline::GraphicPipeline*   graphic_pipeline    = nullptr;

            ~Program()
            {
                delete graphic_pipeline;
                delete descriptor_layout;
                for(auto &d : data) {
                    vkDestroyDescriptorPool(ds_params_.device, d->descriptor_pool, nullptr);
                    delete d->vertex_buffer;
                    delete d->texture.buffer;
                    vkDestroySampler(ds_params_.device, d->texture.sampler, nullptr);
                }
            }

            virtual void init() = 0;
            virtual void addObjData(const GymnureObjData& obj_data) = 0;
        };
    }
}

#endif //GYMNURE_PROGRAM_H
