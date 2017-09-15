#ifndef GYMNURE_H
#define GYMNURE_H

#include <Window/XcbWindow.h>

    class Gymnure
    {
    private:

        Engine::Window::XcbWindow* _window;

    public:

        Gymnure(unsigned int windowWidth, unsigned int windowHeight)
        {
            _window = new Engine::Window::XcbWindow(windowWidth, windowHeight);
            _window->bootstrap();
        }

        void insertData(const char* path, std::string path_obj="", std::vector<VertexData> vertexData = {})
        {
            _window->createCommandBuffer();
            _window->pushTexture(path);
            _window->pushVertex(path_obj, vertexData);
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
