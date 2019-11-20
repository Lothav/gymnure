#include <memory>
#include <chrono>
#include "../gymnure.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    const unsigned windowWidth  = 1500;
    const unsigned windowHeight = 1000;

    {
        auto gymnure = std::make_unique<Gymnure>(windowWidth, windowHeight);

//        auto default_id = gymnure->initDefaultProgram();
        auto phong_id = gymnure->initPhongProgram();
//        auto deferred_id = gymnure->initPhongProgram();

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
//            auto wolf = GymnureObjData{};
//            wolf.paths_textures = {"wolf/textures/Wolf_Fur.jpg"};
//            wolf.obj_path 	    = "wolf/Wolf_One_obj.obj";
//            gymnure->addObjData(phong_id, std::move(wolf));
        }

        {
            auto black_dragon = GymnureObjData{};
            black_dragon.obj_path = "BlackDragon/Dragon 2.5_fbx.fbx";
            gymnure->addFbxData(phong_id, std::move(black_dragon));
        }

        //{
        //    auto quad = GymnureObjData{};
        //    quad.paths_textures = {"baleog.jpg"};
        //    gymnure->addObjData(phong_id, std::move(quad));
        //}

        gymnure->prepare();

        while(true)
        {
            if(!gymnure->draw()) { break; }
        }
    }

    return EXIT_SUCCESS;
}
