#include <memory>
#include <chrono>
#include "../gymnure.h"

int main(int argc, char** argv)
{
    const unsigned windowWidth  = 800;
	const unsigned windowHeight = 600;

	{
		auto gymnure = std::make_unique<Gymnure>(windowWidth, windowHeight);

        auto default_id = gymnure->initDefaultProgram();

        {
            auto room2 = GymnureObjData{};
            room2.obj_path       = "room.obj";
            room2.paths_textures = {"room.png"};
            gymnure->addObjData(default_id, std::move(room2));
        }

		//auto room3 = GymnureObjData{};
		//room3.path_texture 	= "chalet.jpg";
		//room3.obj_path 		= "chalet.obj";
		//gymnure->addObjData(default_id, room3);

		//auto chalet = GymnureObjData{};
		//chalet.path_texture = "chalet.jpg";
		//chalet.obj_path 	= "chalet.obj";
		//gymnure->addObjData(phong_id, chalet);

		//auto cube = GymnureObjData{};
		//cube.obj_path 		= "cube.obj";
		//cube.path_texture 	= "sky.jpg";
		//cube.obj_mtl 		= "cube.mtl";
		//gymnure->addObjData(skybox_id, cube);

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

				Engine::Debug::logInfo("FPS: " + std::to_string(std::round(fps)));

				frames 	 = 0;
				duration = 0.f;
            }
        }
	}

	return EXIT_SUCCESS;
}
