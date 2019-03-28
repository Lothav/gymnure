
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

                // Create Descriptor Set
                descriptor_set = new Descriptors::DescriptorSet(1);
                descriptor_set->create();

                //  Create Uniform Buffer
                uniform_buffer_ = std::make_unique<Descriptors::UniformBuffer>();
                uniform_buffer_->initModelView(app_data->view_width, app_data->view_height);

                // Create Graphics Pipeline
                {
                    auto vert = Engine::GraphicsPipeline::Shader{};
                    vert.type = vk::ShaderStageFlagBits::eVertex;
                    vert.path = "default.vert.spv";

                    auto frag = Engine::GraphicsPipeline::Shader{};
                    frag.type = vk::ShaderStageFlagBits::eFragment;
                    frag.path = "default.frag.spv";

                    graphic_pipeline = new GraphicsPipeline::GraphicsPipeline({vert, frag});
                }
                {
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
                }

                graphic_pipeline->create(descriptor_set->getPipelineLayout(), render_pass, vk::CullModeFlagBits::eNone);
            }

            void addObjData(GymnureObjData&& obj_data) override
            {
                auto app_data = ApplicationData::data;
                auto program_data = std::make_unique<ProgramData>();

                // Load Textures
                for (std::string& texture_path : obj_data.paths_textures) {
                    if(!texture_path.empty())
                        program_data->textures.push_back(
                            std::make_unique<Descriptors::Texture>(texture_path, queue_)) ;
                }

                // Load Vertex
                program_data->vertex_buffer = std::make_unique<Vertex::VertexBuffer>();
                if(!obj_data.obj_path.empty())
                    // Load obj using TinyObjLoader.
                    program_data->vertex_buffer->loadObjModelVertices(obj_data.obj_path, obj_data.obj_mtl);
                else
                    // Empty obj_path. Use triangle as default vertex data.
                    program_data->vertex_buffer->createPrimitiveTriangle();

                data.push_back(program_data);
            }

        };
    }
}

#endif //GYMNURE_DEFAULT_HPP
