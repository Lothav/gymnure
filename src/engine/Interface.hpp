#ifndef GYMNURE_INTERFACE_HPP
#define GYMNURE_INTERFACE_HPP

#include <imgui/imgui.h>
#include <vulkan/vulkan.h>
#include <Descriptors/Texture.hpp>
#include <glm/glm/glm.hpp>
#include <SDL2-2.0.9/include/SDL_timer.h>
#include "Application.hpp"

namespace Engine
{
    class Interface
    {
    public:

        void init()
        {
            // Setup Dear ImGui context
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGui::StyleColorsDark();

            ImGuiIO& io = ImGui::GetIO(); (void)io;

            unsigned char* pixels;
            int width, height;
            io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

            font_image_tex_ = std::make_unique<Descriptors::Texture>(pixels, width, height);
            io.Fonts->TexID = (ImTextureID)(intptr_t)static_cast<VkImage>(font_image_tex_->getImage());

            program_id_ = Application::createInterfaceProgram();
        }

        void prepare(glm::vec2 window_scale, glm::vec2 window_size)
        {
            ImGuiIO& io = ImGui::GetIO();
            IM_ASSERT(io.Fonts->IsBuilt() && "Font atlas not built! It is generally built by the renderer back-end. Missing call to renderer _NewFrame() function? e.g. ImGui_ImplOpenGL3_NewFrame().");

            io.DisplayFramebufferScale = ImVec2(window_scale.x, window_scale.y);
            io.DisplaySize = ImVec2((float)window_size.x, (float)window_size.y);
            // Setup time step (we don't use SDL_GetTicks() because it is using millisecond resolution)
            static uint64_t frequency = SDL_GetPerformanceFrequency();
            uint64_t current_time = SDL_GetPerformanceCounter();
            io.DeltaTime = g_Time > 0 ? (float)((double)(current_time - g_Time) / frequency) : (float)(1.0f / 60.0f);
            g_Time = current_time;
        }

        void render()
        {
            bool show_demo_window = true;
            ImGui::NewFrame();

            ImGui::ShowDemoWindow(&show_demo_window);

            ImGui::EndFrame();
            ImGui::Render();
        }

    private:

        std::unique_ptr<Descriptors::Texture> font_image_tex_;
        uint64_t g_Time = 0;
        uint32_t program_id_ = 0;

    };
}

#endif //GYMNURE_INTERFACE_HPP
