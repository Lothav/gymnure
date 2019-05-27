
#ifndef GYMNURE_DEFAULT_HPP
#define GYMNURE_DEFAULT_HPP

#include <GraphicsPipeline/GraphicsPipeline.h>
#include "Program.h"

namespace Engine
{
    namespace Programs
    {
        struct ProgramData
        {
            std::vector<Engine::GraphicsPipeline::Shader> shaders;
            std::vector<vk::VertexInputAttributeDescription> vi_attribs;
            vk::CullModeFlagBits cull_mode;
        };

        class Default : public Program
        {

        public:

            explicit Default(vk::RenderPass render_pass)
            {
                auto app_data = ApplicationData::data;

                auto ds_data = Descriptors::LayoutData{};
                ds_data.fragment_texture_count = 1;

                // Create Descriptor Set
                descriptor_layout = std::make_unique<Descriptors::Layout>(ds_data);

                // Create Graphics Pipeline
                {
                    auto vert = Engine::GraphicsPipeline::Shader{};
                    vert.type = vk::ShaderStageFlagBits::eVertex;
                    vert.path = "default.vert.spv";

                    auto frag = Engine::GraphicsPipeline::Shader{};
                    frag.type = vk::ShaderStageFlagBits::eFragment;
                    frag.path = "default.frag.spv";

                    std::vector<Engine::GraphicsPipeline::Shader> shaders = {vert, frag};
                    graphic_pipeline = std::make_unique<GraphicsPipeline::GraphicsPipeline>(std::move(shaders));
                }
                {
                    std::vector<vk::VertexInputAttributeDescription> vi_attribs;
                    vi_attribs.resize(2);

                    vi_attribs[0].binding 	= 0;
                    vi_attribs[0].location 	= 0;
                    vi_attribs[0].format 	= vk::Format::eR32G32B32Sfloat;
                    vi_attribs[0].offset 	= static_cast<uint32_t>(offsetof(VertexData, pos));

                    vi_attribs[1].binding 	= 0;
                    vi_attribs[1].location 	= 1;
                    vi_attribs[1].format 	= vk::Format::eR32G32Sfloat;
                    vi_attribs[1].offset 	= static_cast<uint32_t>(offsetof(VertexData, uv));

                    graphic_pipeline->addViAttributes(vi_attribs);
                }

                graphic_pipeline->create(descriptor_layout->getPipelineLayout(), render_pass, vk::CullModeFlagBits::eNone);
            }

        };
    }
}

#endif //GYMNURE_DEFAULT_HPP
