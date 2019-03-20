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

            vk::PipelineCache 						                _pipeline_cache{};
			vk::Pipeline 								            _vk_pipeline{};
			std::vector<vk::PipelineShaderStageCreateInfo,
					mem::StdAllocator<
					        vk::PipelineShaderStageCreateInfo>> 	_shader_stages;
            vk::VertexInputBindingDescription                       _vi_binding{};
            std::vector<vk::VertexInputAttributeDescription>        _vi_attributes;

		public:

			GraphicsPipeline(const std::vector<Shader>& shaders);

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

			void addViAttributes(const vk::VertexInputAttributeDescription& vi_attr);

			void addViAttributes(const std::vector<vk::VertexInputAttributeDescription>& vi_attrs);

			void setViBinding(vk::VertexInputBindingDescription vi_binding);

			void create(vk::PipelineLayout pipeline_layout, vk::RenderPass render_pass, vk::CullModeFlagBits cull_mode);

		};
	}
}


#endif //GRAPHICSPIPELINE_H
