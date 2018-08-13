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

        private:

            struct DescriptorSetParams ds_params_ = {};

        public:

            explicit Skybox(struct DescriptorSetParams ds_params) : ds_params_(std::move(ds_params)) {};

            void init() override
            {
                auto vert = Engine::GraphicPipeline::Shader{};
                vert.type = VK_SHADER_STAGE_VERTEX_BIT;
                vert.path = "../../shaders/skybox.vert.spv";

                auto frag = Engine::GraphicPipeline::Shader{};
                frag.type = VK_SHADER_STAGE_FRAGMENT_BIT;
                frag.path = "../../shaders/skybox.frag.spv";

                descriptor_layout = new Descriptors::DescriptorSet(ds_params_.device, Descriptors::Type::GRAPHIC);
                graphic_pipeline  = new GraphicPipeline::GraphicPipeline(ds_params_.device, {vert, frag});
            }

            void createDescriptorSet() override
            {
                descriptor_layout->create(ds_params_);

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
                graphic_pipeline->create(descriptor_layout->getPipelineLayout(), ds_params_.render_pass, VK_CULL_MODE_FRONT_BIT);
            }

            void addObjData(const GymnureObjData& obj_data) override
            {
                auto* program_data = new ProgramData();

                program_data->descriptor_pool = descriptor_layout->createDescriptorPool();
                program_data->descriptor_set  = descriptor_layout->createDescriptorSet(program_data->descriptor_pool);

                if(!obj_data.path_texture.empty()) {
                    ds_params_.texture_path = obj_data.path_texture;
                    program_data->texture = descriptor_layout->getTextelBuffer(ds_params_);
                }

                // Load Vertex
                std::vector<VertexData> vertexData = {};
                if(!obj_data.path_obj.empty()) vertexData = Vertex::VertexBuffer::loadModelVertices(obj_data.path_obj, obj_data.obj_mtl);
                for(auto v_data : obj_data.vertex_data) vertexData.push_back(v_data);

                struct BufferData vbData = {};
                vbData.device         = ds_params_.device;
                vbData.usage          = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
                vbData.physicalDevice = ds_params_.gpu;
                vbData.properties     = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
                vbData.size           = vertexData.size() * sizeof(VertexData);

                program_data->vertex_buffer = new Vertex::VertexBuffer(vbData, vertexData);

                data.push_back(program_data);

                auto object_id = data.size() - 1;
                if (data[object_id]->texture.buffer != nullptr) {
                    descriptor_layout->updateDescriptorSet(data[object_id]->texture, data[object_id]->descriptor_set);
                }
            }

        };
    }
}

#endif //GYMNURE_SKYBOX_H