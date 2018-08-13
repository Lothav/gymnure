#ifndef GYMNURE_H
#define GYMNURE_H

#include <Window/XcbWindow.h>
#include <Programs/Skybox.h>
#include "GraphicPipeline/GraphicPipeline.h"

struct GymnureData {
    std::string             path_obj        ="";
    std::string             path_texture    ="";
    std::vector<VertexData> vertexData      = {};
    char*                   obj_mtl         = nullptr;
};

class Gymnure
{
private:

    Engine::Window::Window* _window;

    Engine::Programs::Phong*  phong_;
    Engine::Programs::Skybox* skybox_;

public:

    Gymnure(unsigned int windowWidth, unsigned int windowHeight) : phong_(nullptr)
    {
        _window = new Engine::Window::XcbWindow(windowWidth, windowHeight);
    }

    void initPhongProgram()
    {
        if(phong_ != nullptr) {
            std::cerr << "Phong Program already loaded!" << std::endl;
            return;
        }

        phong_ = _window->createPhongProgram();
    }

    void addPhongData(const GymnureData& gymnure_data)
    {
        if(phong_ == nullptr) {
            std::cerr << "Phong Program must be loaded first!" << std::endl;
            return;
        }

        phong_->addObj(gymnure_data.path_obj, gymnure_data.path_texture, gymnure_data.vertexData, gymnure_data.obj_mtl);
    }

    void initSkyboxProgram()
    {
        if(skybox_ != nullptr) {
            std::cerr << "Skybox Program already loaded!" << std::endl;
            return;
        }

        skybox_ = _window->createSkyboxProgram();
    }

    void addSkyboxData(const GymnureData& gymnure_data)
    {
        if(skybox_ == nullptr) {
            std::cerr << "Skybox Program must be loaded first!" << std::endl;
            return;
        }

        skybox_->addObj(gymnure_data.path_obj, gymnure_data.path_texture, gymnure_data.vertexData, gymnure_data.obj_mtl);
    }


    void prepare()
    {
        _window->prepare();
    }

    bool draw()
    {
        bool running = true;
        WindowEvent e = _window->poolEvent();

        if(e == WindowEvent::Close) running = false;

        if(running) _window->draw();

        return running;
    }

    ~Gymnure()
    {
        delete _window;
    }
};

#endif //GYMNURE_GYMNURE_H
