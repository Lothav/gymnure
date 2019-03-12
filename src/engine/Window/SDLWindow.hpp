
#ifndef GYMNURE_SDLWINDOW_HPP
#define GYMNURE_SDLWINDOW_HPP

#include <SDL.h>
#include <SDL_vulkan.h>
#include <ApplicationData.hpp>

namespace Engine
{
    namespace Window
    {
        class SDLWindow
        {

        private:

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

                uint32_t extensions_count = 0;
                SDL_Vulkan_GetInstanceExtensions(window_, &extensions_count, nullptr);
                auto extensions = std::make_shared<std::array<const char*, 2>>();
                SDL_Vulkan_GetInstanceExtensions(window_, &extensions_count, &extensions.get()->front());

                for(auto extension_name : *extensions.get())
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

            void createSurface()
            {
                if (SDL_Vulkan_CreateSurface(window_, ApplicationData::data->instance, &ApplicationData::data->surface) == SDL_bool::SDL_FALSE)
                {
                    std::cerr << "Failed to create Vulkan surface." << std::endl;
                    //assert(false);
                }
            }

        };
    }
}

#endif //GYMNURE_SDLWINDOW_HPP
