#ifndef GYMNURE_SDLWINDOW_HPP
#define GYMNURE_SDLWINDOW_HPP

#include <SDL.h>
#include <SDL_vulkan.h>
#include <SDL_syswm.h>

#include <glm/glm/vec2.hpp>
#include <Descriptors/Camera.h>

namespace Engine::Window
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

        SDLWindow(uint32_t width, uint32_t height);

        ~SDLWindow();

        bool poolEvent(const std::shared_ptr<Descriptors::Camera>& camera);
        void createSurface();

        glm::vec2 getWindowSize();
        glm::vec2 getWindowScale();

        std::vector<const char *> getInstanceExtensionNames();
    };
}

#endif //GYMNURE_SDLWINDOW_HPP
