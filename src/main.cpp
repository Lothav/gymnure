#include "../gymnure.h"

int main(int argc, char** argv) {

	{
		unsigned int windowWidth = 800;
		unsigned int windowHeight = 600;

		Gymnure* gymnure = new Gymnure(windowWidth, windowHeight);

		gymnure->insertData("../../assets/medivh.jpg",
							"",
			{
				{ { -4.0f,  3.0f, 0.0f }, { 0.0f, 1.0f },{ 0.0f, 0.0f, 1.0f } },
				{ { -4.0f, -3.0f, 0.0f }, { 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } },
				{ {  4.0f,  3.0f, 0.0f }, { 1.0f, 1.0f },{ 0.0f, 0.0f, 1.0f } },

				{ {  4.0f,  3.0f, 0.0f }, { 1.0f, 1.0f },{ 0.0f, 0.0f, 1.0f } },
				{ { -4.0f, -3.0f, 0.0f }, { 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } },
				{ {  4.0f, -3.0f, 0.0f }, { 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } }
			}
		);

		gymnure->insertData("../../assets/chalet.jpg", "../../assets/chalet.obj");

		while(1) {
			if(!gymnure->draw()) {
				break;
			}
		}

		delete gymnure;
		return (EXIT_SUCCESS);
	}
}
