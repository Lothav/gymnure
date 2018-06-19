#include "../gymnure.h"

int main(int argc, char** argv) {

	{
		unsigned int windowWidth  = 800;
		unsigned int windowHeight = 600;

		auto* gymnure = new Gymnure(windowWidth, windowHeight);
		//gymnure->insertData("../../assets/sky.jpg", "../../assets/cube.obj");
		gymnure->insertData("../../assets/room2.png", "../../assets/room2.obj");

		while(1) {
			if (!gymnure->draw()) {
				break;
			}
		}

		delete gymnure;
		return (EXIT_SUCCESS);
	}
}
