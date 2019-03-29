//
// Created by luizorv on 9/7/17.
//

#ifndef GRAPHICSPIPELINE_H
#define GRAPHICSPIPELINE_H

#include <vulkan/vulkan.hpp>
#include <array>
#include <Provider.hpp>
#include <Allocator.hpp>
#include "Util/Util.h"

namespace Engine
{
	namespace GraphicsPipeline
	{
		struct Shader {
			std::string 			    path;
			vk::ShaderStageFlagBits 	type;
		};

		class GraphicsPipeline
		{

		private:

            vk::PipelineCache 						                pipeline_cache_{};
			vk::Pipeline 								            pipeline_{};
			std::vector<vk::PipelineShaderStageCreateInfo,
					mem::StdAllocator<
					        vk::PipelineShaderStageCreateInfo>> 	shader_stages_;
            std::vector<vk::VertexInputAttributeDescription>        vi_attributes_;

		public:

			explicit GraphicsPipeline(std::vector<Shader>&& shaders);

            ~GraphicsPipeline();

			void* operator new(std::size_t size)
			{
				return mem::Provider::getMemory(size);
			}

			void operator delete(void* ptr)
			{
				// Do not free memory here!
			}

			vk::Pipeline getPipeline() const;

			void addViAttributes(const std::vector<vk::VertexInputAttributeDescription>& vi_attrs);

			void create(vk::PipelineLayout pipeline_layout, vk::RenderPass render_pass, vk::CullModeFlagBits cull_mode);

		};
	}
}


#endif //GRAPHICSPIPELINE_H
