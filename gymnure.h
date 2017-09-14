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

        bool draw()
        {
            bool running = true;
            while(WindowEvent e = _window->poolEvent()) {
                if(e == WindowEvent::Close) {
                    running = false;
                }
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
