//
// Created by luizorv on 9/3/17.
//

#ifndef OBSIDIAN2D_RENDERPASS_H
#define OBSIDIAN2D_RENDERPASS_H

#include <array>
#include "RenderPass/FrameBuffer.h"

struct rpAttachments {
	VkFormat format;
	bool clear;
};

namespace Engine
{
	namespace RenderPass
	{
		class RenderPass : public FrameBuffer {

		private:

			VkRenderPass _render_pass;

		public:

			RenderPass(VkDevice device, struct SwapChainParams sc_params) : FrameBuffer(device, sc_params) {}

			~RenderPass()
			{
				vkDestroyRenderPass(_sc_params.device, _render_pass, NULL);
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
				attachments[1].storeOp 						= VK_ATTACHMENT_STORE_OP_STORE;
				attachments[1].stencilLoadOp 				= VK_ATTACHMENT_LOAD_OP_LOAD;
				attachments[1].stencilStoreOp 				= VK_ATTACHMENT_STORE_OP_STORE;
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

				VkRenderPassCreateInfo rp_info = {};
				rp_info.sType 								= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
				rp_info.pNext 								= nullptr;
				rp_info.attachmentCount 					= 2;
				rp_info.pAttachments 						= attachments.data();
				rp_info.subpassCount 						= 1;
				rp_info.pSubpasses 							= &subpass;
				rp_info.dependencyCount 					= 0;
				rp_info.pDependencies 						= nullptr;

				assert(vkCreateRenderPass(instance_device, &rp_info, NULL, &_render_pass) == VK_SUCCESS);
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
