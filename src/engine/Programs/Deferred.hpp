#ifndef GYMNURE_DEFERRED_HPP
#define GYMNURE_DEFERRED_HPP

#include "Program.h"

namespace Engine
{
    namespace Programs
    {
        class Deferred : public Program
        {

        public:

            explicit Deferred(vk::RenderPass render_pass)
            {
                auto app_data = ApplicationData::data;

                auto ds_data = Descriptors::DescriptorSetData{};
                ds_data.fragment_texture_count = 1;

                // Create Descriptor Set
                descriptor_set = std::make_unique<Descriptors::DescriptorSet>(ds_data);

                // Create Graphics Pipeline
                {
                    auto vert = Engine::GraphicsPipeline::Shader{};
                    vert.type = vk::ShaderStageFlagBits::eVertex;
                    vert.path = "mrt.vert.spv";

                    auto frag = Engine::GraphicsPipeline::Shader{};
                    frag.type = vk::ShaderStageFlagBits::eFragment;
                    frag.path = "mrt.frag.spv";

                    std::vector<Engine::GraphicsPipeline::Shader> shaders = {vert, frag};
                    graphic_pipeline = std::make_unique<GraphicsPipeline::GraphicsPipeline>(std::move(shaders));
                }
                {
                    std::vector<vk::VertexInputAttributeDescription> vi_attribs;
                    vi_attribs.resize(3);

                    vi_attribs[0].binding 	= 0;
                    vi_attribs[0].location 	= 0;
                    vi_attribs[0].format 	= vk::Format::eR32G32B32Sfloat;
                    vi_attribs[0].offset 	= static_cast<uint32_t>(offsetof(VertexData, pos));

                    vi_attribs[1].binding 	= 0;
                    vi_attribs[1].location 	= 1;
                    vi_attribs[1].format 	= vk::Format::eR32G32Sfloat;
                    vi_attribs[1].offset 	= static_cast<uint32_t>(offsetof(VertexData, uv));

                    vi_attribs[2].binding 	= 0;
                    vi_attribs[2].location 	= 2;
                    vi_attribs[2].format 	= vk::Format::eR32G32B32Sfloat;
                    vi_attribs[2].offset 	= static_cast<uint32_t>(offsetof(VertexData, normal));

                    graphic_pipeline->addViAttributes(vi_attribs);
                }

                graphic_pipeline->create(descriptor_set->getPipelineLayout(), render_pass, vk::CullModeFlagBits::eBack);
            }

            void prepare(const std::shared_ptr<Descriptors::Camera>& camera) override
            {
                auto app_data = ApplicationData::data;

                model_buffer_ = std::make_unique<ModelBuffer>(data.size());

                std::vector<vk::WriteDescriptorSet> writes = {};

                // Create program Descriptor Set.
                auto descriptors_sets = descriptor_set->createDescriptorSets(static_cast<uint32_t>(data.size()));

                for (uint32_t i = 0; i < data.size(); i++)
                {
                    data[i]->descriptor_set = descriptors_sets[i];

                    auto model_bind = model_buffer_->getWrite(descriptors_sets[i], 0);
                    writes.push_back(model_bind);

                    auto camera_binds = camera->getWrites(descriptors_sets[i], 1, 2);
                    writes.push_back(camera_binds[0]);

                    auto texture_bind = data[i]->textures[0]->getWrite(descriptors_sets[i], 3);
                    writes.push_back(texture_bind);

                    writes.push_back(camera_binds[1]);
                }

                app_data->device.updateDescriptorSets(writes, {});
            }

        };
    }
}

#endif //GYMNURE_DEFERRED_HPP
