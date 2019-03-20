//
// Created by luizorv on 9/3/17.
//

#include "RenderPass.h"

namespace Engine
{
    namespace RenderPass
    {
        RenderPass::~RenderPass()
        {
            vkDestroyRenderPass(ApplicationData::data->device, _render_pass, nullptr);
        }

        void RenderPass::create(std::vector<struct rpAttachments> att_vector)
        {
            std::array<vk::AttachmentDescription, 2> attachments = {};
            attachments[0].format 						= att_vector[0].format;
            attachments[0].samples						= vk::SampleCountFlagBits::e1;
            attachments[0].loadOp 						= att_vector[0].clear ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eDontCare;
            attachments[0].storeOp						= vk::AttachmentStoreOp::eStore;
            attachments[0].stencilLoadOp 				= vk::AttachmentLoadOp::eDontCare;
            attachments[0].stencilStoreOp 				= vk::AttachmentStoreOp::eDontCare;
            attachments[0].initialLayout 				= vk::ImageLayout::eUndefined;
            attachments[0].finalLayout 					= vk::ImageLayout::ePresentSrcKHR;

            attachments[1].format 						= att_vector[1].format;
            attachments[1].samples 						= vk::SampleCountFlagBits::e1;
            attachments[1].loadOp 						= att_vector[1].clear ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eDontCare;
            attachments[1].storeOp 						= vk::AttachmentStoreOp::eDontCare;
            attachments[1].stencilLoadOp 				= vk::AttachmentLoadOp::eDontCare;
            attachments[1].stencilStoreOp 				= vk::AttachmentStoreOp::eDontCare;
            attachments[1].initialLayout 				= vk::ImageLayout::eUndefined;
            attachments[1].finalLayout 					= vk::ImageLayout::eDepthStencilAttachmentOptimal;

            vk::AttachmentReference color_reference = {};
            color_reference.attachment 					= 0;
            color_reference.layout 						= vk::ImageLayout::eColorAttachmentOptimal;

            vk::AttachmentReference depth_reference = {};
            depth_reference.attachment 					= 1;
            depth_reference.layout 						= vk::ImageLayout::eDepthStencilAttachmentOptimal;

            vk::SubpassDescription subpass = {};
            subpass.pipelineBindPoint 					= vk::PipelineBindPoint::eGraphics;
            subpass.inputAttachmentCount 				= 0;
            subpass.pInputAttachments 					= nullptr;
            subpass.colorAttachmentCount 				= 1;
            subpass.pColorAttachments 					= &color_reference;
            subpass.pResolveAttachments 				= nullptr;
            subpass.pDepthStencilAttachment 			= &depth_reference;
            subpass.preserveAttachmentCount 			= 0;
            subpass.pPreserveAttachments 				= nullptr;

            // Subpass dependencies for layout transitions
            std::array<vk::SubpassDependency, 2> dependencies = {};

            dependencies[0].srcSubpass 		= VK_SUBPASS_EXTERNAL;
            dependencies[0].dstSubpass 		= 0;
            dependencies[0].srcStageMask 	= vk::PipelineStageFlagBits::eBottomOfPipe;
            dependencies[0].dstStageMask 	= vk::PipelineStageFlagBits::eColorAttachmentOutput;
            dependencies[0].srcAccessMask 	= vk::AccessFlagBits::eMemoryRead;
            dependencies[0].dstAccessMask 	= vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
            dependencies[0].dependencyFlags = vk::DependencyFlagBits::eByRegion;

            dependencies[1].srcSubpass 		= 0;
            dependencies[1].dstSubpass 		= VK_SUBPASS_EXTERNAL;
            dependencies[1].srcStageMask 	= vk::PipelineStageFlagBits::eColorAttachmentOutput;
            dependencies[1].dstStageMask 	= vk::PipelineStageFlagBits::eBottomOfPipe;
            dependencies[1].srcAccessMask 	= vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
            dependencies[1].dstAccessMask 	= vk::AccessFlagBits::eMemoryRead;
            dependencies[1].dependencyFlags = vk::DependencyFlagBits::eByRegion;

            vk::RenderPassCreateInfo rp_info = {};
            rp_info.pNext 								= nullptr;
            rp_info.attachmentCount 					= attachments.size();
            rp_info.pAttachments 						= attachments.data();
            rp_info.subpassCount 						= 1;
            rp_info.pSubpasses 							= &subpass;
            rp_info.dependencyCount 					= dependencies.size();
            rp_info.pDependencies 						= dependencies.data();

            vk::Result res = ApplicationData::data->device.createRenderPass(&rp_info, nullptr, &_render_pass);
            assert(res == vk::Result::eSuccess);
            this->createFrameBuffer(_render_pass);
        }

        vk::RenderPass RenderPass::getRenderPass() const
        {
            return _render_pass;
        }
    }
}
