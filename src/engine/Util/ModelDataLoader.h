#ifndef GYMNURE_MODELDATALOADER_H
#define GYMNURE_MODELDATALOADER_H

#include <unordered_map>
#include "ModelData.hpp"

#ifdef ASSETS_FOLDER_PATH
#define ASSETS_FOLDER_PATH_STR ASSETS_FOLDER_PATH
#else
#define ASSETS_FOLDER_PATH_STR "."
#endif

namespace Engine:: Util
{
    class ModelDataLoader
    {
    public:

        static std::unique_ptr<Model> LoadFBXData(const std::string& model_path);
        static std::unique_ptr<Model> LoadOBJData(const std::string& model_path, const std::string& obj_mtl);
    };
}

#endif //GYMNURE_MODELDATALOADER_H
