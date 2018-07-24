#include <memory>
#include "../gymnure.h"

int main(int argc, char** argv) {

	const unsigned windowWidth  = 800;
	const unsigned windowHeight = 600;

	{
		auto gymnure = std::make_unique<Gymnure>(windowWidth, windowHeight);

		auto room2 = GymnureData{};
		room2.path_texture 	= "../../assets/room2.png";
		room2.path_obj 		= "../../assets/room2.obj";

		gymnure->insertData(room2);

		auto chalet = GymnureData{};
		chalet.path_texture 	= "../../assets/chalet.jpg";
		chalet.path_obj 		= "../../assets/chalet.obj";

		gymnure->insertData(chalet);

		while(true) {
			if (!gymnure->draw()) {
				break;
			}
		}
	}

	return EXIT_SUCCESS;
}
