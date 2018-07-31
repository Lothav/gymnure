#ifndef GYMNURE_H
#define GYMNURE_H

#include <Window/XcbWindow.h>
#include "GraphicPipeline/GraphicPipeline.h"

struct GymnureData {
    std::string path_texture="";
    std::string path_obj="";
    std::vector<VertexData> vertexData = {};
    char* obj_mtl = nullptr;
};

class Gymnure
{
private:

    Engine::Window::Window* _window;

public:

    Gymnure(unsigned int windowWidth, unsigned int windowHeight)
    {
        _window = new Engine::Window::XcbWindow(windowWidth, windowHeight);
        _window->createDescriptorSet();
    }

    void addObject(const GymnureData& gymnureData)
    {
        _window->addObj(Engine::ProgramType::OBJECT, gymnureData.path_obj, gymnureData.path_texture, gymnureData.vertexData, gymnureData.obj_mtl);
    }

    void addSkybox(const GymnureData& gymnureData)
    {
        _window->addObj(Engine::ProgramType::SKYBOX, gymnureData.path_obj, gymnureData.path_texture, gymnureData.vertexData, gymnureData.obj_mtl);
    }

    void insertText(const GymnureData& gymnureData)
    {}

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
