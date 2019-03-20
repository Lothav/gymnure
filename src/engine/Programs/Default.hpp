
#ifndef GYMNURE_DEFAULT_HPP
#define GYMNURE_DEFAULT_HPP

#include <GraphicsPipeline/GraphicsPipeline.h>
#include "Program.h"

namespace Engine
{
    namespace Programs
    {
        class Default : public Program
        {

        public:

            explicit Default(vk::Queue queue)
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

            void init(vk::RenderPass render_pass) override
            {
                auto app_data = ApplicationData::data;

                auto vert = Engine::GraphicsPipeline::Shader{};
                vert.type = vk::ShaderStageFlagBits::eVertex;
                vert.path = "default.vert.spv";

                auto frag = Engine::GraphicsPipeline::Shader{};
                frag.type = vk::ShaderStageFlagBits::eFragment;
                frag.path = "default.frag.spv";

                descriptor_set = new Descriptors::DescriptorSet(0);
                graphic_pipeline = new GraphicsPipeline::GraphicsPipeline({vert, frag});

                descriptor_set->create();

                vk::VertexInputBindingDescription vi_binding = {};
                vi_binding.binding 					= 0;
                vi_binding.inputRate 				= vk::VertexInputRate::eVertex;
                vi_binding.stride 					= sizeof(VertexData);

                std::vector<vk::VertexInputAttributeDescription> vi_attribs;
                vi_attribs.resize(2);

                vi_attribs[0].binding 			    = 0;
                vi_attribs[0].location 			    = 0;
                vi_attribs[0].format 			    = vk::Format::eR32G32B32Sfloat;
                vi_attribs[0].offset 			    = static_cast<uint32_t>(offsetof(VertexData, pos));

                vi_attribs[1].binding 			    = 0;
                vi_attribs[1].location 			    = 1;
                vi_attribs[1].format 			    = vk::Format::eR32G32Sfloat;
                vi_attribs[1].offset 			    = static_cast<uint32_t>(offsetof(VertexData, uv));

                graphic_pipeline->addViAttributes(vi_attribs);
                graphic_pipeline->setViBinding(vi_binding);
                graphic_pipeline->create(descriptor_set->getPipelineLayout(), render_pass, vk::CullModeFlagBits::eNone);
            }

            void addObjData(const GymnureObjData& obj_data) override
            {
                auto app_data = ApplicationData::data;
                auto* program_data = new ProgramData();

                program_data->descriptor_pool = descriptor_set->createDescriptorPool();
                program_data->descriptor_set  = descriptor_set->createDescriptorSet(program_data->descriptor_pool);

                if(!obj_data.path_texture.empty())
                    program_data->texture = descriptor_set->getTextelBuffer(obj_data.path_texture, queue_);

                // Load Vertex
                program_data->vertex_buffer = new Vertex::VertexBuffer();

                if(!obj_data.path_obj.empty())
                    program_data->vertex_buffer->loadObjModelVertices(obj_data.path_obj, obj_data.obj_mtl);
                else
                    program_data->vertex_buffer->createPrimitiveTriangle();

                descriptor_set->updateDescriptorSet(program_data->texture, program_data->descriptor_set);

                data.push_back(program_data);
            }

        };
    }
}

#endif //GYMNURE_DEFAULT_HPP
