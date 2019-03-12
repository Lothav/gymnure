#ifndef GYMNURE_H
#define GYMNURE_H

#include <Programs/Skybox.h>
#include "GraphicPipeline/GraphicPipeline.h"
#include <Provider.hpp>
#include <Window/SDLWindow.hpp>
#include <Application.hpp>

#define GB (1024 * 1024 * 1024)

class Gymnure
{
private:

    Engine::Window::SDLWindow* window_;

    Engine::Programs::Phong*  phong_;
    Engine::Programs::Skybox* skybox_{};

public:

    Gymnure(unsigned int windowWidth, unsigned int windowHeight) : phong_(nullptr)
    {
        mem::Provider::initPool(1*GB);
        window_ = new Engine::Window::SDLWindow(windowWidth, windowHeight);
        Engine::Application::create(window_->getInstanceExtensionNames());
        window_->createSurface();
        Engine::Application::setupSurface(windowWidth, windowHeight);
    }

    ~Gymnure()
    {
        delete window_;
        mem::Provider::destroyPool();
    }

    void initPhongProgram()
    {
        if(phong_ != nullptr) {
            std::cerr << "Phong Program already loaded!" << std::endl;
            return;
        }

        phong_ = Engine::Application::createPhongProgram();
    }

    void addPhongData(const GymnureObjData& gymnure_data)
    {
        if(phong_ == nullptr) {
            std::cerr << "Phong Program must be loaded first!" << std::endl;
            return;
        }

        phong_->addObjData(gymnure_data);
    }

    void initSkyboxProgram()
    {
        if(skybox_ != nullptr) {
            std::cerr << "Skybox Program already loaded!" << std::endl;
            return;
        }

        skybox_ = Engine::Application::createSkyboxProgram();
    }

    void addSkyboxData(const GymnureObjData& gymnure_data)
    {
        if(skybox_ == nullptr) {
            std::cerr << "Skybox Program must be loaded first!" << std::endl;
            return;
        }

        skybox_->addObjData(gymnure_data);
    }

    void prepare()
    {
        Engine::Application::prepare();
    }

    bool draw()
    {
        ///if(Engine::Application::poolEvent() == WindowEvent::Close)
        ///    return false;

        Engine::Application::draw();
        return true;
    }

};

#endif //GYMNURE_GYMNURE_H
