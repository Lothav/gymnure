
#ifndef GYMNURE_SDLWINDOW_HPP
#define GYMNURE_SDLWINDOW_HPP

#include <SDL.h>
#include <SDL_vulkan.h>

#include <vulkan/vulkan.h>
#include <SDL2/SDL_syswm.h>

#include <ApplicationData.hpp>
#include <Application.hpp>

namespace Engine
{
    namespace Window
    {
        class SDLWindow
        {

        private:

            SDL_SysWMinfo info{};
            SDL_Window* window_ = nullptr;
            std::vector<const char *> instance_extension_names_ = {};
            uint32_t width;
            uint32_t height;

        public:

            SDLWindow(u_int32_t width, u_int32_t height)
            {
                // Setup SDL
                if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
                {
                    printf("Error trying to init SDL window: %s\n", SDL_GetError());
                    return;
                }

                this->width = width;
                this->height = height;

                // Setup window
                SDL_DisplayMode current;
                SDL_GetCurrentDisplayMode(0, &current);
                auto window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

                window_ = SDL_CreateWindow("Hell Yeah!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, window_flags);

                SDL_VERSION(&info.version);   // initialize info structure with SDL version info
                if(!SDL_GetWindowWMInfo(window_, &info))
                    Debug::logErrorAndDie(std::string("Couldn't get window information: ") + SDL_GetError());

                uint32_t extensions_count = 0;
                SDL_Vulkan_GetInstanceExtensions(window_, &extensions_count, nullptr);
                auto extensions = std::vector<const char*>();
                extensions.resize(extensions_count);
                SDL_Vulkan_GetInstanceExtensions(window_, &extensions_count, extensions.data());

                for(auto extension_name : extensions)
                    instance_extension_names_.emplace_back(extension_name);

            #ifdef DEBUG
                instance_extension_names_.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            #endif
            }

            ~SDLWindow()
            {
                SDL_DestroyWindow(window_);
                SDL_Quit();
            }

            std::vector<const char *> getInstanceExtensionNames()
            {
                return instance_extension_names_;
            }

            bool mouse_down = false;

            bool poolEvent()
            {
                SDL_Event event;
                while (SDL_PollEvent(&event))
                {
                    //ImGui_ImplSDL2_ProcessEvent(&event);

                    if(event.type == SDL_MOUSEBUTTONDOWN)
                        mouse_down = true;

                    if(event.type == SDL_MOUSEBUTTONUP)
                        mouse_down = false;

                    if(mouse_down && event.type == SDL_MOUSEMOTION)
                    {
                        // Get the mouse offsets
                        float x = (float) event.motion.xrel / width;
                        float y = (float) event.motion.yrel / height;

                        if (x != 0 && y != 0)
                            Application::getMainCamera()->rotateArcballCamera(x, y);
                    }

                    if(event.type == SDL_MOUSEWHEEL)
                    {
                        // Get the mouse offsets
                        float zoom = -event.wheel.y;
                        Application::getMainCamera()->zoomCamera(zoom);
                    }

                    switch(event.type)
                    {
                        case SDL_QUIT:
                            return false;

                        case SDL_KEYDOWN:
                            switch (event.key.keysym.sym) {
                                case SDLK_UP:
                                    Application::getMainCamera()->moveCamera(glm::vec3(0.0f, 1.f, 0));
                                    break;
                                case SDLK_DOWN:
                                    Application::getMainCamera()->moveCamera(glm::vec3(0.0f, -1.f, 0));
                                    break;
                                case SDLK_LEFT:
                                    Application::getMainCamera()->moveCamera(glm::vec3(1.f, 0.0f, 0));
                                    break;
                                case SDLK_RIGHT:
                                    Application::getMainCamera()->moveCamera(glm::vec3(-1.f, 0.0f, 0));
                                    break;
                                default:
                                    break;
                            }

                            //auto uniform_buffer = program->descriptor_set->getUniformBuffer();
                            //uniform_buffer->zoomCamera(zoom);
                            break;

                        default:
                            break;
                    }
                    //if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED && event.window.windowID == SDL_GetWindowID(window_))
                    //    ImGui_ImplVulkanH_CreateWindowDataSwapChainAndFramebuffer(g_PhysicalDevice, g_Device, &g_WindowData, g_Allocator, (int)event.window.data1, (int)event.window.data2);
                    }

                return true;
            }

            void createSurface()
            {
                VkSurfaceKHR temp_surf;
                if (SDL_Vulkan_CreateSurface(window_, ApplicationData::data->instance, &temp_surf) == SDL_bool::SDL_FALSE)
                    Debug::logErrorAndDie(std::string("Failed to create Vulkan surface: ") + SDL_GetError());

                ApplicationData::data->surface = temp_surf;
            }

        };
    }
}

#endif //GYMNURE_SDLWINDOW_HPP
