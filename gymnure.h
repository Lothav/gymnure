#ifndef GYMNURE_H
#define GYMNURE_H

#include <Window/XcbWindow.h>
#include <Programs/Skybox.h>
#include "GraphicPipeline/GraphicPipeline.h"
#include <memancpp/Provider.hpp>

#define GB (1024 * 1024 * 1024)

class Gymnure
{
private:

    Engine::Window::Window* _window;

    Engine::Programs::Phong*  phong_;
    Engine::Programs::Skybox* skybox_;

public:

    Gymnure(unsigned int windowWidth, unsigned int windowHeight) : phong_(nullptr)
    {
        Memory::Provider::initPool(1*GB);
        _window = new Engine::Window::XcbWindow(windowWidth, windowHeight);
    }

    ~Gymnure()
    {
        Memory::Provider::destroyPool();
        delete _window;
    }

    void initPhongProgram()
    {
        if(phong_ != nullptr) {
            std::cerr << "Phong Program already loaded!" << std::endl;
            return;
        }

        phong_ = _window->createPhongProgram();
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

        skybox_ = _window->createSkyboxProgram();
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
        _window->prepare();
    }

    bool draw()
    {
        WindowEvent e = _window->poolEvent();
        if(e == WindowEvent::Close) return false;

        _window->draw();

        return true;
    }

};

#endif //GYMNURE_GYMNURE_H
