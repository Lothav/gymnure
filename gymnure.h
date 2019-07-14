#ifndef GYMNURE_H
#define GYMNURE_H

#include <Window/SDLWindow.hpp>
#include <Application.hpp>
#include <Util/Debug.hpp>
#include <cmath>

class Gymnure
{
private:

    std::unique_ptr<Engine::Window::SDLWindow> window_ = nullptr;

    uint32_t frame_count = 0;
    float frame_duration = 0.f;

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
        Engine::Application::setupPipelines();
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
        Engine::Application::addObjData(program_id, std::move(gymnure_data));
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
