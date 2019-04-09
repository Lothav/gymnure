
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

            bool mouse_down = false;

        public:

            SDLWindow(u_int32_t width, u_int32_t height);

            ~SDLWindow();

            bool poolEvent();
            void createSurface();

            std::vector<const char *> getInstanceExtensionNames();
        };
    }
}

#endif //GYMNURE_SDLWINDOW_HPP
