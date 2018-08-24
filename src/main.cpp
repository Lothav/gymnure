
#include <memory>
#include <chrono>
#include "../gymnure.h"

int main(int argc, char** argv) {

	const unsigned windowWidth  = 800;
	const unsigned windowHeight = 600;

	{
		auto gymnure = std::make_unique<Gymnure>(windowWidth, windowHeight);

        gymnure->initPhongProgram();
		gymnure->initSkyboxProgram();

        auto room2 = GymnureObjData{};
		room2.path_texture 	= "../assets/room2.png";
		room2.path_obj 		= "../assets/room2.obj";
        gymnure->addPhongData(room2);

		//auto chalet = GymnureData{};
		//chalet.path_texture = "../../assets/chalet.jpg";
		//chalet.path_obj 	= "../../assets/chalet.obj";
		//gymnure->addObject(chalet);

		auto cube = GymnureObjData{};
		cube.path_obj 		= "../assets/cube.obj";
		cube.path_texture 	= "../assets/sky.jpg";
		cube.obj_mtl 		= const_cast<char*>(std::string("../assets/cube.mtl").data());
		gymnure->addSkyboxData(cube);

		gymnure->prepare();

		uint frames		= 0;
		float duration  = 0.f;
		while(true) {

            auto start = std::chrono::high_resolution_clock::now();
			if(!gymnure->draw()) break;
            auto end = std::chrono::high_resolution_clock::now();

            frames 	 += 1;
            duration += std::chrono::duration<double, std::milli>(end - start).count();

            // Update FPS every 3 sec
            if (duration >= 3e3) {
				auto fps = std::abs((float)frames * (1.e3 / duration));

				std::cout << fps << std::endl;

				frames 	 = 0;
				duration = 0.f;
            }
        }
	}

	return EXIT_SUCCESS;
}
