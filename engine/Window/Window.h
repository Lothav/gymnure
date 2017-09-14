//
// Created by luizorv on 7/22/17.
//

#ifndef OBSIDIAN2D_CORE_WINDOW2_H
#define OBSIDIAN2D_CORE_WINDOW2_H

#include "Descriptors/UniformBuffer.h"
#include "Memory/Memory.h"
#include "Descriptors/Textures.h"
#include "Memory/BufferImage.h"
#include "Vertex/VertexBuffer.h"
#include "RenderPass/RenderPass.h"
#include "Descriptors/DescriptorSet.h"
#include "SyncPrimitives/SyncPrimitives.h"
#include "CommandBuffers.h"
#include "GraphicPipeline/GraphicPipeline.h"

#define APP_NAME "Obsidian2D"

enum WindowEvent
{
    None       = 0,
    Click      = 1,
    Focus      = 2,
    Blur       = 3,
    Resize     = 4,
    Close      = 5,
    ButtonDown = 6,
    ButtonUp   = 7,
    Unknow     = 8,
};


namespace Gymnure
{
    namespace Window
    {
        class Window : public Util::Layers {

        public:

            VkInstance 		instance;
            VkSurfaceKHR 	surface;

            ~Window() {
                uint32_t i;

                delete render_pass;

                if (surface != VK_NULL_HANDLE) {
                    vkDestroySurfaceKHR(instance, surface, nullptr);
                }

                for (i = 0; i < descriptor_set.size(); i++)
                {
                    delete descriptor_set[i];
                }
                for (i = 0; i < graphic_pipeline.size(); i++)
                {
                    delete graphic_pipeline[i];
                }
                for (i = 0; i < vertex_buffer.size(); i++)
                {
                    delete vertex_buffer[i];
                }
                delete sync_primitives;

                for (i = 0; i < command_buffer.size(); i++)
                {
                    delete command_buffer[i];
                }

				vkDestroyCommandPool(device, command_pool, nullptr);

                for (i = 0; i < Descriptors::Textures::textureImageMemory.size(); i++)
                {
                    vkFreeMemory(device, Descriptors::Textures::textureImageMemory[i], nullptr);
                }

                vkDestroyDevice(device, nullptr);
                vkDestroyInstance(instance, nullptr);
            }

            void draw()
            {
                VkResult res;
                VkSwapchainKHR swap_c = render_pass->getSwapChain()->getSwapChainKHR();

                res = vkAcquireNextImageKHR(device, swap_c, UINT64_MAX, sync_primitives->imageAcquiredSemaphore, VK_NULL_HANDLE, &current_buffer);
                assert(res == VK_SUCCESS);

                VkPipelineStageFlags pipe_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

                std::vector<VkCommandBuffer> cmd_buff = {};
                for (int i = 0; i < command_buffer.size(); ++i)
                {
					descriptor_set[ i ]->getUniformBuffer()->updateMVP(device);
					cmd_buff.push_back(command_buffer[i]->getCommandBuffer());
                }

                VkSubmitInfo submit_info;
                submit_info.pNext                     = NULL;
                submit_info.sType                     = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                submit_info.waitSemaphoreCount        = 1;
                submit_info.pWaitSemaphores           = &sync_primitives->imageAcquiredSemaphore;
                submit_info.pWaitDstStageMask         = &pipe_stage_flags;
                submit_info.commandBufferCount        = static_cast<uint32_t>(cmd_buff.size());
                submit_info.pCommandBuffers           = cmd_buff.data();
                submit_info.signalSemaphoreCount      = 1;
                submit_info.pSignalSemaphores         = &sync_primitives->renderSemaphore;

                res = vkQueueSubmit(render_pass->getSwapChain()->getGraphicQueue(), 1, &submit_info, *sync_primitives->getFence(current_buffer));
                assert(res == VK_SUCCESS);

                do {
                    res = vkWaitForFences(device, 1, sync_primitives->getFence(current_buffer), VK_TRUE, VK_SAMPLE_COUNT_1_BIT);
                } while (res == VK_TIMEOUT);
                assert(res == VK_SUCCESS);
                vkResetFences(device, 1, sync_primitives->getFence(current_buffer));

                VkPresentInfoKHR present;

                present.sType 				           = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
                present.pNext 				           = NULL;
                present.swapchainCount 		           = 1;
                present.pSwapchains 		           = &swap_c;
                present.pImageIndices 		           = &current_buffer;
                present.pWaitSemaphores 	           = NULL;
                present.waitSemaphoreCount 	           = 0;
                present.pResults                       = NULL;

                if (sync_primitives->renderSemaphore != VK_NULL_HANDLE)
                {
                    present.pWaitSemaphores = &sync_primitives->renderSemaphore;
                    present.waitSemaphoreCount = 1;
                }

                res = vkQueuePresentKHR(render_pass->getSwapChain()->getPresentQueue(), &present);
                assert(res == VK_SUCCESS);
                cmd_buff.clear();
            }

        private:

            VkDevice 								            device;
            VkPhysicalDeviceMemoryProperties 		            memory_properties;
            uint32_t 								            current_buffer = 0;
			u_int32_t								            cm_count = 0;
			std::vector<VkPhysicalDevice> 			            gpu_vector;
			u_int32_t							 	            queue_family_count;
			std::vector<VkQueueFamilyProperties> 	            queue_family_props;
			u_int32_t                                           queueFamilyIndex = UINT_MAX;
			VkCommandPool 							            command_pool;

            std::vector<GraphicPipeline::GraphicPipeline *>     graphic_pipeline;
            std::vector<CommandBuffers *>			            command_buffer;
            SyncPrimitives::SyncPrimitives* 					sync_primitives;
            RenderPass::RenderPass* 							render_pass;

        protected:

            std::vector<Vertex::VertexBuffer *>             vertex_buffer;
            std::vector<Descriptors::DescriptorSet*> 			descriptor_set;

            void createApplication()
            {
                std::vector<const char *> _layer_names = this->getLayerNames();
                std::vector<const char *> _instance_extension_names;

                _instance_extension_names.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
                //@TODO extension for win32 VK_KHR_WIN32_SURFACE_EXTENSION_NAME
                _instance_extension_names.push_back("VK_KHR_xcb_surface");

                VkApplicationInfo _app_info = {};
                _app_info.sType 				= VK_STRUCTURE_TYPE_APPLICATION_INFO;
                _app_info.pNext 				= NULL;
                _app_info.pApplicationName 		= APP_NAME;
                _app_info.applicationVersion 	= 1;
                _app_info.pEngineName 			= APP_NAME;
                _app_info.engineVersion 		= 1;
                _app_info.apiVersion 			= VK_API_VERSION_1_0;

                VkInstanceCreateInfo _inst_info = {};
                _inst_info.sType 					= VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
                _inst_info.pNext 					= NULL;
                _inst_info.flags 					= 0;
                _inst_info.pApplicationInfo 		= &_app_info;
                _inst_info.enabledLayerCount 		= (uint32_t) _layer_names.size();
                _inst_info.ppEnabledLayerNames 		= _layer_names.size() ? _layer_names.data() : NULL;
                _inst_info.enabledExtensionCount 	= (uint32_t) _instance_extension_names.size();
                _inst_info.ppEnabledExtensionNames 	= _instance_extension_names.data();

                VkResult res = vkCreateInstance(&_inst_info, NULL, &instance);
                assert(res == VK_SUCCESS);

                res = vkEnumeratePhysicalDevices(instance, &queue_family_count, NULL);
                assert(res == VK_SUCCESS && queue_family_count);
                gpu_vector.resize(queue_family_count);
                res = vkEnumeratePhysicalDevices(instance, &queue_family_count, gpu_vector.data());
                assert(res == VK_SUCCESS);

                vkGetPhysicalDeviceQueueFamilyProperties(gpu_vector[0], &queue_family_count, NULL);
                assert(queue_family_count >= 1);

                queue_family_props.resize(queue_family_count);
                vkGetPhysicalDeviceQueueFamilyProperties(gpu_vector[0], &queue_family_count, queue_family_props.data());
                assert(queue_family_count >= 1);

                vkGetPhysicalDeviceMemoryProperties(gpu_vector[0], &memory_properties);

                bool found = false;
                for (unsigned int i = 0; i < queue_family_count; i++) {
                    if (queue_family_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                        queueFamilyIndex = i;
                        found = true;
                        break;
                    }
                }
                assert(found && queueFamilyIndex != UINT_MAX);

                float queue_priorities[1] = {0.0};

                VkDeviceQueueCreateInfo queue_info = {};
                queue_info.sType 			= VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queue_info.pNext 			= NULL;
                queue_info.queueCount 		= 1;
                queue_info.pQueuePriorities = queue_priorities;
                queue_info.queueFamilyIndex = queueFamilyIndex;

                std::vector<const char *> device_extension_names;
                device_extension_names.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

                VkDeviceCreateInfo device_info = {};
                device_info.sType 					= VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
                device_info.pNext 					= NULL;
                device_info.queueCreateInfoCount 	= 1;
                device_info.pQueueCreateInfos 		= &queue_info;
                device_info.enabledExtensionCount 	= (uint32_t)device_extension_names.size();
                device_info.ppEnabledExtensionNames = device_info.enabledExtensionCount ? device_extension_names.data() : NULL;
                device_info.enabledLayerCount 		= 0;
                device_info.ppEnabledLayerNames 	= NULL;
                device_info.pEnabledFeatures 		= NULL;

                res = vkCreateDevice(gpu_vector[0], &device_info, NULL, &device);
                assert(res == VK_SUCCESS);

				VkCommandPoolCreateInfo cmd_pool_info = {};
				cmd_pool_info.sType 			= VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
				cmd_pool_info.pNext 			= nullptr;
				cmd_pool_info.queueFamilyIndex  = queueFamilyIndex;
				cmd_pool_info.flags 			= 0;

				assert(vkCreateCommandPool(device, &cmd_pool_info, nullptr, &command_pool) == VK_SUCCESS);
			}

            void initGraphicPipeline ()
            {
                VkResult U_ASSERT_ONLY 	res;

                /* Render Pass */

                struct SwapChainParams sc_params = {};
                sc_params.gpu 					= gpu_vector[0];
                sc_params.width 				= static_cast<u_int32_t >(width);
                sc_params.height 				= static_cast<u_int32_t >(height);
                sc_params.device 				= device;
                sc_params.queue_family_count 	= queue_family_count;
                sc_params.queue_family_props 	= queue_family_props;
                sc_params.surface 				= surface;
                sc_params.memory_props 			= memory_properties;

                render_pass = new RenderPass::RenderPass(device, sc_params);

                std::vector< struct rpAttachments > rp_attachments = {};
                struct rpAttachments attch = {};
                attch.format = render_pass->getSwapChain()->getSwapChainFormat();
                attch.clear  = false;
                rp_attachments.push_back(attch);
                attch.format = render_pass->getDepthBufferFormat();
                attch.clear  = true;
                rp_attachments.push_back(attch);

                render_pass->create(rp_attachments);

				sync_primitives = new SyncPrimitives::SyncPrimitives(device);
				sync_primitives->createSemaphore();
				sync_primitives->createFence(render_pass->getSwapChain()->getImageCount());
            }

			void createCommandBuffer()
			{
				command_buffer.push_back( new CommandBuffers(device, queueFamilyIndex, command_pool) );
			}

            void pushTexture(const char* path)
            {

                descriptor_set.push_back( new Descriptors::DescriptorSet(device) );

                struct DescriptorSetParams ds_params = {};
                ds_params.width 				= static_cast<u_int32_t>(width);
                ds_params.height 				= static_cast<u_int32_t>(height);
                ds_params.memory_properties		= memory_properties;
                ds_params.command_pool			= command_pool;
                ds_params.gpu					= gpu_vector[0];
                ds_params.graphic_queue			= render_pass->getSwapChain()->getGraphicQueue();
                ds_params.path                  = path;

                descriptor_set[ cm_count ]->create(ds_params);

                graphic_pipeline.push_back( new GraphicPipeline::GraphicPipeline(device) );
                graphic_pipeline[ cm_count ]->
						create( descriptor_set[ cm_count ]->getPipelineLayout(), render_pass->getRenderPass() );
            }

			void pushVertex(std::vector<VertexData> vertexData)
			{
				struct BufferData vertexBufferData = {};

				vertexBufferData.device            = device;
				vertexBufferData.usage             = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
				vertexBufferData.physicalDevice    = gpu_vector[0];
				vertexBufferData.properties        = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
				vertexBufferData.size              = vertexData.size() * sizeof(VertexData);

				vertex_buffer.push_back( new Vertex::VertexBuffer(vertexBufferData, vertexData) );
			}

            void recordCommandBuffer()
            {
                command_buffer[ cm_count ]
                    ->bindCommandBuffer (
                        render_pass,
                        descriptor_set[ cm_count ],
                        graphic_pipeline[ cm_count ]->getPipeline(),
                        static_cast<uint32_t>(width),
                        static_cast<uint32_t>(height),
                        sync_primitives,
                        vertex_buffer[ cm_count ]
                    );

				cm_count++;
            }

        };
    }
}
#endif //OBSIDIAN2D_CORE_WINDOW2_H