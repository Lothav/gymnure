
#ifndef GYMNURE_SDLWINDOW_HPP
#define GYMNURE_SDLWINDOW_HPP

#define VK_USE_PLATFORM_XCB_KHR
#define SDL_VIDEO_DRIVER_X11

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

            SDL_SysWMinfo info;
            SDL_Window* window_ = nullptr;
            std::vector<const char *> instance_extension_names_ = {};

        public:

            SDLWindow(u_int32_t width, u_int32_t height)
            {
                // Setup SDL
                if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0)
                {
                    printf("Error: %s\n", SDL_GetError());
                    return;
                }

                // Setup window
                SDL_DisplayMode current;
                SDL_GetCurrentDisplayMode(0, &current);
                auto window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

                window_ = SDL_CreateWindow("Hell Yeah!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, window_flags);

                SDL_VERSION(&info.version);   // initialize info structure with SDL version info
                if(!SDL_GetWindowWMInfo(window_, &info))
                {
                    Debug::logError(std::string("Couldn't get window information: ") + SDL_GetError());
                }

                uint32_t extensions_count = 0;
                SDL_Vulkan_GetInstanceExtensions(window_, &extensions_count, nullptr);
                auto extensions = std::vector<const char*>();
                extensions.resize(extensions_count);
                SDL_Vulkan_GetInstanceExtensions(window_, &extensions_count, extensions.data());

                for(auto extension_name : extensions)
                    instance_extension_names_.emplace_back(extension_name);

            #ifdef DEBUG
                instance_extension_names_.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
                instance_extension_names_.emplace_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
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

            bool poolEvent()
            {
                SDL_Event event;
                while (SDL_PollEvent(&event))
                {
                    //ImGui_ImplSDL2_ProcessEvent(&event);
                    switch( event.type ) {
                        case SDL_QUIT:
                            return false;

                        case SDL_KEYDOWN:

                            auto programs = Application::getPrograms();

                            auto zoom = 0.0f;
                            switch (event.key.keysym.sym) {
                                case SDLK_UP:
                                    zoom = 0.1f;
                                    break;
                                case SDLK_DOWN:
                                    zoom = -0.1f;
                                    break;
                                default:
                                    break;
                            }

                            for (auto program : programs) {
                                auto uniform_buffer = program->descriptor_set->getUniformBuffer();
                                uniform_buffer->zoomCamera(zoom);
                            }
                            break;
                        }
                    //if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED && event.window.windowID == SDL_GetWindowID(window_))
                    //    ImGui_ImplVulkanH_CreateWindowDataSwapChainAndFramebuffer(g_PhysicalDevice, g_Device, &g_WindowData, g_Allocator, (int)event.window.data1, (int)event.window.data2);
                    }

                return true;
            }

            void createSurface()
            {
                if (SDL_Vulkan_CreateSurface(window_, ApplicationData::data->instance, &ApplicationData::data->surface) == SDL_bool::SDL_FALSE)
                {
                    Debug::logError(std::string("Failed to create Vulkan surface: ") + SDL_GetError());
                }
            }

        };
    }
}

#endif //GYMNURE_SDLWINDOW_HPP
