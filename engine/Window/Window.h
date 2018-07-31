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
#include "Util/Layers.h"

#define APP_NAME "Obsidian2D"

namespace Engine
{
    namespace Window
    {

        class Window : public Util::Layers {

        public:

            VkInstance 		instance;
            VkSurfaceKHR 	surface;

            virtual ~Window()
            {
                uint32_t i;

                delete render_pass;

                if (surface != VK_NULL_HANDLE) vkDestroySurfaceKHR(instance, surface, nullptr);

                delete command_buffer;

                for(auto& [key, program_obj]: programs) {
                    delete program_obj.graphic_pipeline;
                    for (auto &data : program_obj.data) {
                        delete data->descriptor_set;
                        delete data->vertex_buffer;
                        delete data->texture.buffer;
                        vkDestroySampler(device, data->texture.sampler, nullptr);
                    }
                }

                delete sync_primitives;

                vkDestroyCommandPool(device, graphic_command_pool, nullptr);

                for (i = 0; i < Descriptors::Textures::textureImageMemory.size(); i++) {
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
                {
                    cmd_buff.push_back(command_buffer->getCommandBuffer());
                    for(auto& [key, program_obj]: programs) {
                        for (auto &data : program_obj.data){
                            data->descriptor_set->getUniformBuffer()->updateMVP();
                        }
                    }
                }

                VkSubmitInfo submit_info = {};
                submit_info.pNext                     = nullptr;
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

                VkPresentInfoKHR present = {};

                present.sType 				           = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
                present.pNext 				           = nullptr;
                present.swapchainCount 		           = 1;
                present.pSwapchains 		           = &swap_c;
                present.pImageIndices 		           = &current_buffer;
                present.pWaitSemaphores 	           = nullptr;
                present.waitSemaphoreCount 	           = 0;
                present.pResults                       = nullptr;

                if (sync_primitives->renderSemaphore != VK_NULL_HANDLE) {
                    present.pWaitSemaphores = &sync_primitives->renderSemaphore;
                    present.waitSemaphoreCount = 1;
                }

                res = vkQueuePresentKHR(render_pass->getSwapChain()->getPresentQueue(), &present);
                assert(res == VK_SUCCESS);
                cmd_buff.clear();
            }

        private:

            VkQueue                                             compute_queue_;
            VkDevice 								            device;
            uint32_t 								            current_buffer = 0;
            u_int32_t							 	            queue_family_count;
            u_int32_t                                           queueGraphicFamilyIndex = UINT_MAX;
            u_int32_t                                           queueComputeFamilyIndex = UINT_MAX;
            RenderPass::RenderPass* 							render_pass;
            std::vector<VkPhysicalDevice> 			            gpu_vector;
            SyncPrimitives::SyncPrimitives* 					sync_primitives;
            VkPhysicalDeviceMemoryProperties 		            memory_properties;
            std::vector<VkQueueFamilyProperties> 	            queue_family_props;

            VkCommandPool 							            graphic_command_pool;
            CommandBuffers*                                     command_buffer = nullptr;

        protected:

            std::map<ProgramType, Program>                      programs;

            void init()
            {
                this->initGraphicPipeline();

                {
                    auto vert = Engine::GraphicPipeline::Shader{};
                    vert.type = VK_SHADER_STAGE_VERTEX_BIT;
                    vert.path = "../../shaders/phong.vert.spv";

                    auto frag = Engine::GraphicPipeline::Shader{};
                    frag.type = VK_SHADER_STAGE_FRAGMENT_BIT;
                    frag.path = "../../shaders/phong.frag.spv";

                    auto* program_obj = &programs[ProgramType::OBJECT];
                    program_obj->graphic_pipeline = new GraphicPipeline::GraphicPipeline(device, {vert, frag});
                }

                {
                    auto vert = Engine::GraphicPipeline::Shader{};
                    vert.type = VK_SHADER_STAGE_VERTEX_BIT;
                    vert.path = "../../shaders/skybox.vert.spv";

                    auto frag = Engine::GraphicPipeline::Shader{};
                    frag.type = VK_SHADER_STAGE_FRAGMENT_BIT;
                    frag.path = "../../shaders/skybox.frag.spv";

                    auto* program_obj = &programs[ProgramType::SKYBOX];
                    program_obj->graphic_pipeline = new GraphicPipeline::GraphicPipeline(device, {vert, frag});
                }

                command_buffer = new CommandBuffers(device, graphic_command_pool);
            }

            void createApplication()
            {
                std::vector<const char *> _layer_names = this->getLayerNames();
                std::vector<const char *> _instance_extension_names;

                _instance_extension_names.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
                //@TODO extension for win32 VK_KHR_WIN32_SURFACE_EXTENSION_NAME
                _instance_extension_names.push_back("VK_KHR_xcb_surface");

                VkApplicationInfo _app_info = {};
                _app_info.sType 				= VK_STRUCTURE_TYPE_APPLICATION_INFO;
                _app_info.pNext 				= nullptr;
                _app_info.pApplicationName 		= APP_NAME;
                _app_info.applicationVersion 	= 1;
                _app_info.pEngineName 			= APP_NAME;
                _app_info.engineVersion 		= 1;
                _app_info.apiVersion 			= VK_API_VERSION_1_0;

                VkInstanceCreateInfo _inst_info = {};
                _inst_info.sType 					= VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
                _inst_info.pNext 					= nullptr;
                _inst_info.flags 					= 0;
                _inst_info.pApplicationInfo 		= &_app_info;
                _inst_info.enabledLayerCount 		= (uint32_t) _layer_names.size();
                _inst_info.ppEnabledLayerNames 		= _layer_names.size() ? _layer_names.data() : nullptr;
                _inst_info.enabledExtensionCount 	= (uint32_t) _instance_extension_names.size();
                _inst_info.ppEnabledExtensionNames 	= _instance_extension_names.data();

                VkResult res = vkCreateInstance(&_inst_info, nullptr, &instance);
                assert(res == VK_SUCCESS);

                res = vkEnumeratePhysicalDevices(instance, &queue_family_count, nullptr);
                assert(res == VK_SUCCESS && queue_family_count);
                gpu_vector.resize(queue_family_count);
                res = vkEnumeratePhysicalDevices(instance, &queue_family_count, gpu_vector.data());
                assert(res == VK_SUCCESS);

                vkGetPhysicalDeviceQueueFamilyProperties(gpu_vector[0], &queue_family_count, nullptr);
                assert(queue_family_count >= 1);

                queue_family_props.resize(queue_family_count);
                vkGetPhysicalDeviceQueueFamilyProperties(gpu_vector[0], &queue_family_count, queue_family_props.data());
                assert(queue_family_count >= 1);

                vkGetPhysicalDeviceMemoryProperties(gpu_vector[0], &memory_properties);

                bool foundGraphic = false;
                bool foundCompute = false;
                for (unsigned int i = 0; i < queue_family_count; i++) {
                    if (queue_family_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                        queueGraphicFamilyIndex = i;
                        foundGraphic = true;
                    }

                    // Some GPU's have a dedicate compute queue. Try to find it.
                    if ((queue_family_props[i].queueFlags & VK_QUEUE_COMPUTE_BIT) && ((queue_family_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)) {
                        queueComputeFamilyIndex = i;
                        foundCompute = true;
                    }
                }

                // If no able to find a compute queue dedicated one, find a generic that support compute.
                if (!foundCompute) {
                    for (unsigned int i = 0; i < queue_family_count; i++) {
                        if (queue_family_props[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
                            queueComputeFamilyIndex = i;
                            foundCompute = true;
                        }
                    }
                }

                assert(foundGraphic && foundCompute && queueComputeFamilyIndex != UINT_MAX && queueGraphicFamilyIndex != UINT_MAX);

                float queue_priorities[1] = {0.0};

                VkDeviceQueueCreateInfo queue_info = {};
                queue_info.sType 			= VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queue_info.pNext 			= nullptr;
                queue_info.queueCount 		= 1;
                queue_info.pQueuePriorities = queue_priorities;
                queue_info.queueFamilyIndex = queueGraphicFamilyIndex;

                std::vector<const char *> device_extension_names;
                device_extension_names.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

                VkDeviceCreateInfo device_info = {};
                device_info.sType 					= VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
                device_info.pNext 					= nullptr;
                device_info.queueCreateInfoCount 	= 1;
                device_info.pQueueCreateInfos 		= &queue_info;
                device_info.enabledExtensionCount 	= (uint32_t)device_extension_names.size();
                device_info.ppEnabledExtensionNames = device_info.enabledExtensionCount ? device_extension_names.data() : nullptr;
                device_info.enabledLayerCount 		= 0;
                device_info.ppEnabledLayerNames 	= nullptr;
                device_info.pEnabledFeatures 		= nullptr;

                res = vkCreateDevice(gpu_vector[0], &device_info, nullptr, &device);
                assert(res == VK_SUCCESS);

                VkCommandPoolCreateInfo cmd_pool_info = {};
                cmd_pool_info.sType 			= VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
                cmd_pool_info.pNext 			= nullptr;
                cmd_pool_info.queueFamilyIndex  = queueGraphicFamilyIndex;
                cmd_pool_info.flags 			= 0;

                assert(vkCreateCommandPool(device, &cmd_pool_info, nullptr, &graphic_command_pool) == VK_SUCCESS);

                vkGetDeviceQueue(device, queueComputeFamilyIndex, 0, &compute_queue_);
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

        public:

            virtual WindowEvent poolEvent()
            {
                return WindowEvent::None;
            }

            void createDescriptorSet()
            {
                {
                    auto* program_obj = &programs[ProgramType::OBJECT];

                    // Create ProgramType::OBJECT Graphic Pipeline
                    VkVertexInputBindingDescription vi_binding = {};
                    vi_binding.binding 					= 0;
                    vi_binding.inputRate 				= VK_VERTEX_INPUT_RATE_VERTEX;
                    vi_binding.stride 					= sizeof(VertexData);

                    std::vector<VkVertexInputAttributeDescription> vi_attribs;
                    vi_attribs.resize(3);

                    vi_attribs[0].binding 			    = 0;
                    vi_attribs[0].location 			    = 0;
                    vi_attribs[0].format 			    = VK_FORMAT_R32G32B32_SFLOAT;
                    vi_attribs[0].offset 			    = static_cast<uint32_t>(offsetof(VertexData, pos));

                    vi_attribs[1].binding 			    = 0;
                    vi_attribs[1].location 			    = 1;
                    vi_attribs[1].format 			    = VK_FORMAT_R32G32_SFLOAT;
                    vi_attribs[1].offset 			    = static_cast<uint32_t>(offsetof(VertexData, uv));

                    vi_attribs[2].binding 			    = 0;
                    vi_attribs[2].location 			    = 2;
                    vi_attribs[2].format 			    = VK_FORMAT_R32G32B32_SFLOAT;
                    vi_attribs[2].offset 				= static_cast<uint32_t>(offsetof(VertexData, normal));

                    program_obj->graphic_pipeline->addViAttributes(vi_attribs);
                    program_obj->graphic_pipeline->setViBinding(vi_binding);
                    program_obj->graphic_pipeline->create(program_obj->descriptor_set->getPipelineLayout(), render_pass->getRenderPass(), VK_CULL_MODE_BACK_BIT);

                }

                {
                    auto* program_obj = &programs[ProgramType::SKYBOX];

                    // Create ProgramType::OBJECT Graphic Pipeline
                    VkVertexInputBindingDescription vi_binding = {};
                    vi_binding.binding 					= 0;
                    vi_binding.inputRate 				= VK_VERTEX_INPUT_RATE_VERTEX;
                    vi_binding.stride 					= sizeof(VertexData);

                    std::vector<VkVertexInputAttributeDescription> vi_attribs;
                    vi_attribs.resize(3);

                    vi_attribs[0].binding 			    = 0;
                    vi_attribs[0].location 			    = 0;
                    vi_attribs[0].format 			    = VK_FORMAT_R32G32B32_SFLOAT;
                    vi_attribs[0].offset 			    = static_cast<uint32_t>(offsetof(VertexData, pos));

                    vi_attribs[1].binding 			    = 0;
                    vi_attribs[1].location 			    = 1;
                    vi_attribs[1].format 			    = VK_FORMAT_R32G32_SFLOAT;
                    vi_attribs[1].offset 			    = static_cast<uint32_t>(offsetof(VertexData, uv));

                    vi_attribs[2].binding 			    = 0;
                    vi_attribs[2].location 			    = 2;
                    vi_attribs[2].format 			    = VK_FORMAT_R32G32B32_SFLOAT;
                    vi_attribs[2].offset 				= static_cast<uint32_t>(offsetof(VertexData, normal));

                    program_obj->graphic_pipeline->addViAttributes(vi_attribs);
                    program_obj->graphic_pipeline->setViBinding(vi_binding);
                    program_obj->graphic_pipeline->create(program_obj->descriptor_set->getPipelineLayout(), render_pass->getRenderPass(), VK_CULL_MODE_FRONT_BIT);
                }

            }

            void addObj(const ProgramType program_type = ProgramType::OBJECT, const std::string& path_obj = "", const std::string& path_texture = "", std::vector<VertexData> complementVertexData = {}, const char* obj_mtl = nullptr)
            {
                auto* program_obj = &programs[program_type];

                auto* program_data = new ProgramData();

                program_data->descriptor_set = new Descriptors::DescriptorSet(device, Descriptors::Type::GRAPHIC);

                struct DescriptorSetParams ds_params = {};
                ds_params.width 				    = static_cast<u_int32_t>(width);
                ds_params.height 				    = static_cast<u_int32_t>(height);
                ds_params.memory_properties		    = memory_properties;
                ds_params.command_pool			    = graphic_command_pool;
                ds_params.gpu					    = gpu_vector[0];
                ds_params.graphic_queue			    = render_pass->getSwapChain()->getGraphicQueue();

                program_data->descriptor_set->create(ds_params);

                // Load Texture
                if (!path_texture.empty()) {
                    ds_params.texture_path = path_texture;
                    program_data->texture = program_data->descriptor_set->getTextelBuffer(ds_params);
                }

                // Load Vertex
                std::vector<VertexData> vertexData = {};
                if(!path_obj.empty()) {
                    vertexData = Vertex::VertexBuffer::loadModelVertices(path_obj, obj_mtl);
                }

                for (auto v_data : complementVertexData) {
                    vertexData.push_back(v_data);
                }

                struct BufferData vertexBufferData = {};

                vertexBufferData.device            = device;
                vertexBufferData.usage             = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
                vertexBufferData.physicalDevice    = gpu_vector[0];
                vertexBufferData.properties        = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
                vertexBufferData.size              = vertexData.size() * sizeof(VertexData);

                program_data->vertex_buffer = new Vertex::VertexBuffer(vertexBufferData, vertexData);

                addProgramData(program_type, program_data);
            }

            void addProgramData(const ProgramType program_type, ProgramData* program_data)
            {
                programs[program_type].data.push_back(program_data);

                auto object_id = programs[program_type].data.size()-1;

                recordCommandBuffer(object_id, program_type);
            }

            void recordCommandBuffer(const ulong data_id, const ProgramType& type)
            {
                auto* program_obj = &programs[type];

                if (data_id >= program_obj->data.size()) assert(false);

                if (program_obj->data[data_id]->texture.buffer != nullptr) {
                    program_obj->data[data_id]->descriptor_set->updateDescriptorSet(program_obj->data[data_id]->texture);
                }
            }

            void prepare()
            {
                command_buffer->bindGraphicCommandBuffer(programs, render_pass, static_cast<uint32_t>(width), static_cast<uint32_t>(height), sync_primitives);
            }

        };
    }
}
#endif //OBSIDIAN2D_CORE_WINDOW2_H