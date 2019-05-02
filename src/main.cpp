#include <memory>
#include <chrono>
#include "../gymnure.h"

int main(int argc, char** argv)
{
    const unsigned windowWidth  = 800;
	const unsigned windowHeight = 600;

	{
		auto gymnure = std::make_unique<Gymnure>(windowWidth, windowHeight);

        //auto default_id = gymnure->initDefaultProgram();
        auto phong_id = gymnure->initPhongProgram();

        //{
        //    auto room2 = GymnureObjData{};
        //    room2.obj_path       = "room.obj";
        //    room2.paths_textures = {"room.png"};
        //    gymnure->addObjData(phong_id, std::move(room2));
        //}

        //{
        //    auto chalet = GymnureObjData{};
        //    chalet.paths_textures = {"chalet.jpg"};
        //    chalet.obj_path 	  = "chalet.obj";
        //    gymnure->addObjData(phong_id, std::move(chalet));
        //}

        {
            auto wolf = GymnureObjData{};
            wolf.paths_textures = {"wolf/textures/Wolf_Body.jpg"};
            wolf.obj_path 	    = "wolf/Wolf_One_obj.obj";
            gymnure->addObjData(phong_id, std::move(wolf));
        }

        gymnure->prepare();

		while(true) {
			if(!gymnure->draw()) break;
        }
	}

	return EXIT_SUCCESS;
}
