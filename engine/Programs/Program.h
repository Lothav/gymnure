//
// Created by luiz0tavio on 8/12/18.
//

#ifndef GYMNURE_PROGRAM_H
#define GYMNURE_PROGRAM_H

#include <GraphicPipeline/GraphicPipeline.h>
#include <Descriptors/DescriptorSet.h>
#include <Vertex/VertexBuffer.h>

namespace Engine
{
    namespace Programs
    {
        struct ProgramData {
            Descriptors::Texture                texture             = {};
            Vertex::VertexBuffer*               vertex_buffer       = nullptr;
            VkDescriptorPool                    descriptor_pool     = nullptr;
            VkDescriptorSet                     descriptor_set      = nullptr;
        };

        class Program
        {

        public:

            virtual void init(VkDevice device) = 0;

            virtual void createDescriptorSet(struct DescriptorSetParams ds_params) = 0;

        protected:

            Descriptors::DescriptorSet*         descriptor_layout   = nullptr;
            std::vector<ProgramData*>           data                = {};
            GraphicPipeline::GraphicPipeline*   graphic_pipeline    = nullptr;

        };
    }
}

#endif //GYMNURE_PROGRAM_H
