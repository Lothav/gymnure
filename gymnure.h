#ifndef GYMNURE_H
#define GYMNURE_H

#include <Window/SDLWindow.hpp>
#include <Descriptors/Texture.hpp>
#include <Application.hpp>
#include <Util/Debug.hpp>
#include <cmath>

class Gymnure
{
private:

    std::unique_ptr<Engine::Window::SDLWindow> window_ = nullptr;

    uint32_t frame_count = 0;
    double frame_duration = 0.0;

public:

    Gymnure(unsigned int windowWidth, unsigned int windowHeight)
    {
        window_ = std::make_unique<Engine::Window::SDLWindow>(windowWidth, windowHeight);
        Engine::Application::create(window_->getInstanceExtensionNames());
#ifdef DEBUG
        Engine::Debug::init();
#endif
        window_->createSurface();
        Engine::Application::setupSurface(windowWidth, windowHeight);

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGuiIO& io = ImGui::GetIO(); (void)io;

        unsigned char* pixels;
        int width, height;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
        // @TODO save font_image_tex reference and properly destroy it.
        auto font_image_tex = new Engine::Descriptors::Texture(pixels, width, height);
        io.Fonts->TexID = (ImTextureID)(intptr_t)static_cast<VkImage>(font_image_tex->getImage());
    }

    ~Gymnure()
    {
#ifdef DEBUG
        Engine::Application::destroy();
#endif
    }

    uint32_t initPhongProgram()
    {
        return Engine::Application::createPhongProgram();
    }

    uint32_t initDeferredProgram()
    {
        return Engine::Application::createDeferredProgram();
    }

    void addObjData(uint32_t program_id, GymnureObjData&& gymnure_data)
    {
        Engine::Application::addObjData(program_id, std::move(gymnure_data), GymnureObjDataType::OBJ);
    }

    void addFbxData(uint32_t program_id, GymnureObjData&& gymnure_data)
    {
        Engine::Application::addObjData(program_id, std::move(gymnure_data), GymnureObjDataType::FBX);
    }

    void prepare()
    {
        Engine::Application::prepare();
    }

    bool draw()
    {
        if(!window_->poolEvent())
            return false;
#if defined(DEBUG)
        auto start = std::chrono::high_resolution_clock::now();
        TRY_CATCH_BLOCK_FN(Engine::Application::draw());
        auto end = std::chrono::high_resolution_clock::now();

        frame_count += 1;
        frame_duration += std::chrono::duration<double, std::milli>(end - start).count();

        // Update FPS every sec
        if (frame_duration >= 1e3) {
            auto fps = std::abs((float)frame_count * (1.e3 / frame_duration));

            Engine::Debug::logInfo("FPS: " + std::to_string(std::round(fps)));

            frame_count     = 0;
            frame_duration  = 0.f;
        }
#else
        Engine::Application::draw();
#endif
        return true;
    }

};

#endif //GYMNURE_GYMNURE_H
