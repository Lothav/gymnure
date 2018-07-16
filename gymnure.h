#ifndef GYMNURE_H
#define GYMNURE_H

#include <Window/XcbWindow.h>

class Gymnure
{
private:

    Engine::Window::Window* _window;

public:

    Gymnure(unsigned int windowWidth, unsigned int windowHeight)
    {
        _window = new Engine::Window::XcbWindow(windowWidth, windowHeight);
    }

    void insertData(const char* path_texture, std::string path_obj="", std::vector<VertexData> vertexData = {}, const char* obj_mtl = nullptr)
    {
        _window->createCommandBuffers();
        _window->createDescriptorSet(path_texture);
        _window->pushVertex(path_obj, vertexData, obj_mtl);
        _window->recordCommandBuffer();
    }

    bool draw()
    {
        bool running = true;
        WindowEvent e = _window->poolEvent();
        if(e == WindowEvent::Close) {
            running = false;
        }
        if(running) {
            _window->draw();
        }
        return running;
    }

    ~Gymnure()
    {
        delete _window;
    }
};

#endif //GYMNURE_GYMNURE_H
