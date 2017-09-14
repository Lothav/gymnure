#include "../gymnure.h"

int main(int argc, char** argv) {

	{
		unsigned int windowWidth = 800;
		unsigned int windowHeight = 600;

        Gymnure* gymnure = new Gymnure(windowWidth, windowHeight);

		while(1) {
            if(!gymnure->draw()){
                break;
            }
        }

		delete gymnure;
		return (EXIT_SUCCESS);
	}
}
