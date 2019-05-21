//
// Created by luizorv on 9/3/17.
//

#include <ApplicationData.hpp>
#include "RenderPass.h"

namespace Engine
{
    namespace RenderPass
    {
        RenderPass::~RenderPass()
        {
            vkDestroyRenderPass(ApplicationData::data->device, render_pass_, nullptr);
        }

        RenderPass::RenderPass(std::vector<RpAttachments> att_vector)
        {
            std::vector<vk::AttachmentDescription> attachments = {};
            std::vector<vk::AttachmentReference> color_references = {};
            vk::AttachmentReference depth_reference = {};

            uint32_t idx = 0;
            for (auto& att_vec : att_vector)
            {
                vk::AttachmentDescription attachment{};
                attachment.format           = att_vec.format;
                attachment.samples			= vk::SampleCountFlagBits::e1;
                attachment.loadOp 			= att_vec.clear ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eDontCare;
                attachment.storeOp			= vk::AttachmentStoreOp::eStore;
                attachment.stencilLoadOp 	= vk::AttachmentLoadOp::eDontCare;
                attachment.stencilStoreOp 	= vk::AttachmentStoreOp::eDontCare;
                attachment.initialLayout 	= vk::ImageLayout::eUndefined;
                attachment.finalLayout 		= att_vec.final_layout; //vk::ImageLayout::ePresentSrcKHR / vk::ImageLayout::eDepthStencilAttachmentOptimal;
                attachments.push_back(std::move(attachment));

                if (att_vec.usage & vk::ImageUsageFlagBits::eDepthStencilAttachment)
                    depth_reference = {idx, vk::ImageLayout::eColorAttachmentOptimal};
                else
                    color_references.emplace_back(idx, vk::ImageLayout::eDepthStencilAttachmentOptimal);

                idx++;
            }

            vk::SubpassDescription subpass = {};
            subpass.pipelineBindPoint 					= vk::PipelineBindPoint::eGraphics;
            subpass.inputAttachmentCount 				= 0;
            subpass.pInputAttachments 					= nullptr;
            subpass.colorAttachmentCount 				= static_cast<uint32_t>(color_references.size());
            subpass.pColorAttachments 					= color_references.data();
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
            rp_info.attachmentCount 					= static_cast<uint32_t>(attachments.size());
            rp_info.pAttachments 						= attachments.data();
            rp_info.subpassCount 						= 1;
            rp_info.pSubpasses 							= &subpass;
            rp_info.dependencyCount 					= dependencies.size();
            rp_info.pDependencies 						= dependencies.data();

            render_pass_ = ApplicationData::data->device.createRenderPass(rp_info);
        }

        vk::RenderPass RenderPass::getRenderPass() const
        {
            return render_pass_;
        }
    }
}
