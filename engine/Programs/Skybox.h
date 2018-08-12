//
// Created by luiz0tavio on 8/12/18.
//

#ifndef GYMNURE_SKYBOX_H
#define GYMNURE_SKYBOX_H

#include "Program.h"

namespace Engine
{
    namespace Programs
    {
        class Skybox : public Program
        {

        public:

            void init(VkDevice device) override
            {
                auto vert = Engine::GraphicPipeline::Shader{};
                vert.type = VK_SHADER_STAGE_VERTEX_BIT;
                vert.path = "../../shaders/skybox.vert.spv";

                auto frag = Engine::GraphicPipeline::Shader{};
                frag.type = VK_SHADER_STAGE_FRAGMENT_BIT;
                frag.path = "../../shaders/skybox.frag.spv";

                descriptor_layout = new Descriptors::DescriptorSet(device, Descriptors::Type::GRAPHIC);
                graphic_pipeline  = new GraphicPipeline::GraphicPipeline(device, {vert, frag});
            }

            void createDescriptorSet(struct DescriptorSetParams ds_params) override
            {
                VkVertexInputBindingDescription vi_binding = {};
                vi_binding.binding 					= 0;
                vi_binding.inputRate 				= VK_VERTEX_INPUT_RATE_VERTEX;
                vi_binding.stride 					= sizeof(VertexData);

                std::vector<VkVertexInputAttributeDescription> vi_attribs;
                vi_attribs.resize(3);

                vi_attribs[0].binding 			    = 0;
                vi_attribs[0].location 			    = 0;
                vi_attribs[0].format 			    = VK_FORMAT_R32G32B32_SFLOAT;
                vi_attribs[0].offset 			    = static_cast<uint32_t>(offsetof(VertexData, pos));

                vi_attribs[1].binding 			    = 0;
                vi_attribs[1].location 			    = 1;
                vi_attribs[1].format 			    = VK_FORMAT_R32G32_SFLOAT;
                vi_attribs[1].offset 			    = static_cast<uint32_t>(offsetof(VertexData, uv));

                vi_attribs[2].binding 			    = 0;
                vi_attribs[2].location 			    = 2;
                vi_attribs[2].format 			    = VK_FORMAT_R32G32B32_SFLOAT;
                vi_attribs[2].offset 				= static_cast<uint32_t>(offsetof(VertexData, normal));

                graphic_pipeline->addViAttributes(vi_attribs);
                graphic_pipeline->setViBinding(vi_binding);
                graphic_pipeline->create(descriptor_layout->getPipelineLayout(), ds_params.render_pass, VK_CULL_MODE_FRONT_BIT);
            }

        };
    }
}

#endif //GYMNURE_SKYBOX_H
