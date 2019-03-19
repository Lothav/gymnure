//
// Created by luizorv on 9/3/17.
//

#ifndef OBSIDIAN2D_RENDERPASS_H
#define OBSIDIAN2D_RENDERPASS_H

#include <array>
#include <Application.hpp>
#include "RenderPass/FrameBuffer.h"

struct rpAttachments
{
	VkFormat format;
	bool clear;
};

namespace Engine
{
	namespace RenderPass
	{
		class RenderPass : public FrameBuffer {

		private:

			VkRenderPass _render_pass{};

		public:

			RenderPass() : FrameBuffer() {}

			~RenderPass()
			{
				vkDestroyRenderPass(ApplicationData::data->device, _render_pass, nullptr);
			}

			void* operator new(std::size_t size)
			{
				return mem::Provider::getMemory(size);
			}

			void operator delete(void* ptr)
			{
				// Do not free memory here!
			}

			void create(std::vector<struct rpAttachments> att_vector)
			{
				std::array<VkAttachmentDescription, 2> attachments = {};
				attachments[0].format 						= att_vector[0].format;
				attachments[0].samples						= VK_SAMPLE_COUNT_1_BIT;
				attachments[0].loadOp 						= att_vector[0].clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				attachments[0].storeOp						= VK_ATTACHMENT_STORE_OP_STORE;
				attachments[0].stencilLoadOp 				= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				attachments[0].stencilStoreOp 				= VK_ATTACHMENT_STORE_OP_DONT_CARE;
				attachments[0].initialLayout 				= VK_IMAGE_LAYOUT_UNDEFINED;
				attachments[0].finalLayout 					= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
				attachments[0].flags 						= 0;

				attachments[1].format 						= att_vector[1].format;
				attachments[1].samples 						= VK_SAMPLE_COUNT_1_BIT;
				attachments[1].loadOp 						= att_vector[1].clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				attachments[1].storeOp 						= VK_ATTACHMENT_STORE_OP_DONT_CARE;
				attachments[1].stencilLoadOp 				= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				attachments[1].stencilStoreOp 				= VK_ATTACHMENT_STORE_OP_DONT_CARE;
				attachments[1].initialLayout 				= VK_IMAGE_LAYOUT_UNDEFINED;
				attachments[1].finalLayout 					= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
				attachments[1].flags 						= 0;

				VkAttachmentReference color_reference = {};
				color_reference.attachment 					= 0;
				color_reference.layout 						= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

				VkAttachmentReference depth_reference = {};
				depth_reference.attachment 					= 1;
				depth_reference.layout 						= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

				VkSubpassDescription subpass = {};
				subpass.pipelineBindPoint 					= VK_PIPELINE_BIND_POINT_GRAPHICS;
				subpass.flags 								= 0;
				subpass.inputAttachmentCount 				= 0;
				subpass.pInputAttachments 					= nullptr;
				subpass.colorAttachmentCount 				= 1;
				subpass.pColorAttachments 					= &color_reference;
				subpass.pResolveAttachments 				= nullptr;
				subpass.pDepthStencilAttachment 			= &depth_reference;
				subpass.preserveAttachmentCount 			= 0;
				subpass.pPreserveAttachments 				= nullptr;

				// Subpass dependencies for layout transitions
				std::array<VkSubpassDependency, 2> dependencies = {};

				dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
				dependencies[0].dstSubpass = 0;
				dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
				dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
				dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

				dependencies[1].srcSubpass = 0;
				dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
				dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
				dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
				dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

				VkRenderPassCreateInfo rp_info = {};
				rp_info.sType 								= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
				rp_info.pNext 								= nullptr;
				rp_info.attachmentCount 					= attachments.size();
				rp_info.pAttachments 						= attachments.data();
				rp_info.subpassCount 						= 1;
				rp_info.pSubpasses 							= &subpass;
				rp_info.dependencyCount 					= dependencies.size();
				rp_info.pDependencies 						= dependencies.data();

				VkResult res = vkCreateRenderPass(ApplicationData::data->device, &rp_info, nullptr, &_render_pass);
				assert(res == VK_SUCCESS);
				this->createFrameBuffer(_render_pass);
			}

			VkRenderPass getRenderPass()
			{
				return _render_pass;
			}
		};
	}
}

#endif //OBSIDIAN2D_RENDERPASS_H
