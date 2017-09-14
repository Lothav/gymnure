#include <Window/XcbWindow.h>

int main(int argc, char** argv) {

	{
		unsigned int windowWidth = 800;
		unsigned int windowHeight = 600;

		Gymnure::Window::XcbWindow* xcbWindow = new Gymnure::Window::XcbWindow(windowWidth, windowHeight);
		xcbWindow->bootstrap();

		bool running = true;
		while(running) {
			while(WindowEvent e = xcbWindow->poolEvent()) {
				if(e == WindowEvent::Close) {
					running = false;
				} else {
				}
			}
			xcbWindow->draw();
		}

		delete xcbWindow;
		xcbWindow = nullptr;

		return (EXIT_SUCCESS);
	}
}
