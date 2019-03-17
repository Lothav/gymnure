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

            explicit Skybox(VkQueue queue)
            {
                queue_ = queue;
            }

            void* operator new(std::size_t size)
            {
                return mem::Provider::getMemory(size);
            }

            void operator delete(void* ptr)
            {
                // Do not free memory here!
            }

            void init(VkRenderPass render_pass) override
            {
                auto app_data = ApplicationData::data;

                auto vert = Engine::GraphicPipeline::Shader{};
                vert.type = VK_SHADER_STAGE_VERTEX_BIT;
                vert.path = "skybox.vert.spv";

                auto frag = Engine::GraphicPipeline::Shader{};
                frag.type = VK_SHADER_STAGE_FRAGMENT_BIT;
                frag.path = "skybox.frag.spv";

                descriptor_set = new Descriptors::DescriptorSet(Descriptors::Type::GRAPHIC);
                graphic_pipeline  = new GraphicPipeline::GraphicPipeline(app_data->device, {vert, frag});

                descriptor_set->create();

                VkVertexInputBindingDescription vi_binding = {};
                vi_binding.binding 		= 0;
                vi_binding.inputRate 	= VK_VERTEX_INPUT_RATE_VERTEX;
                vi_binding.stride 		= sizeof(VertexData);

                std::vector<VkVertexInputAttributeDescription> vi_attribs;
                vi_attribs.resize(3);

                vi_attribs[0].binding 	= 0;
                vi_attribs[0].location 	= 0;
                vi_attribs[0].format 	= VK_FORMAT_R32G32B32_SFLOAT;
                vi_attribs[0].offset 	= static_cast<uint32_t>(offsetof(VertexData, pos));

                vi_attribs[1].binding 	= 0;
                vi_attribs[1].location 	= 1;
                vi_attribs[1].format 	= VK_FORMAT_R32G32_SFLOAT;
                vi_attribs[1].offset 	= static_cast<uint32_t>(offsetof(VertexData, uv));

                vi_attribs[2].binding 	= 0;
                vi_attribs[2].location 	= 2;
                vi_attribs[2].format 	= VK_FORMAT_R32G32B32_SFLOAT;
                vi_attribs[2].offset 	= static_cast<uint32_t>(offsetof(VertexData, normal));

                graphic_pipeline->addViAttributes(vi_attribs);
                graphic_pipeline->setViBinding(vi_binding);
                graphic_pipeline->create(descriptor_set->getPipelineLayout(), render_pass, VK_CULL_MODE_FRONT_BIT);
            }

            void addObjData(const GymnureObjData& obj_data) override
            {
               /* auto app_data = ApplicationData::data;
                auto* program_data = new ProgramData();

                program_data->descriptor_pool = descriptor_set->createDescriptorPool();
                program_data->descriptor_set  = descriptor_set->createDescriptorSet(program_data->descriptor_pool);

                if(!obj_data.path_texture.empty()) {
                    program_data->texture = descriptor_set->getTextelBuffer(obj_data.path_texture, queue_);
                }

                // Load Vertex
                std::vector<VertexData> vertexData = {};
                if(!obj_data.path_obj.empty()) vertexData = Vertex::VertexBuffer::loadObjModelVertices(obj_data.path_obj, obj_data.obj_mtl);
                for(auto v_data : obj_data.vertex_data) vertexData.push_back(v_data);

                struct BufferData vbData = {};
                vbData.usage          = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
                vbData.properties     = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
                vbData.size           = vertexData.size() * sizeof(VertexData);

                program_data->vertex_buffer = new Vertex::VertexBuffer(vbData, vertexData);

                if(program_data->texture.buffer != nullptr) {
                    descriptor_set->updateDescriptorSet(program_data->texture, program_data->descriptor_set);
                }

                data.push_back(program_data);*/
            }

        };
    }
}

#endif //GYMNURE_SKYBOX_H
