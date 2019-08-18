#ifndef GYMNURE_MODELLOADER_H
#define GYMNURE_MODELLOADER_H

#include <string>
#include <unordered_map>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#ifdef ASSETS_FOLDER_PATH
#define ASSETS_FOLDER_PATH_STR ASSETS_FOLDER_PATH
#else
#define ASSETS_FOLDER_PATH_STR "."
#endif

namespace Engine
{
    struct VertexData
    {
        glm::vec3 pos;
        glm::vec2 uv;
        glm::vec3 normal;

        bool operator ==(const VertexData& other) const
        {
            return pos == other.pos && uv == other.uv;
        }
    };

    struct Material
    {
        std::string texture_path;
    };

    struct Mesh
    {
        std::unique_ptr<std::vector<std::unique_ptr<VertexData>>> vertexData;
        std::unique_ptr<Material>                material;
    };

    struct Model
    {
        std::unique_ptr<std::vector<std::unique_ptr<Mesh>>> meshes;
    };

    namespace Util
    {
        class ModelLoader
        {
        public:
            static std::unique_ptr<Model> LoadFBXData(const std::string& assets_model_path);
        };
    }
}

template<> struct std::hash<Engine::VertexData>
{
    size_t operator()(Engine::VertexData const& vertex) const
    {
        return std::hash<glm::vec3>()(vertex.pos) ^ (std::hash<glm::vec2>()(vertex.uv) << 1);
    }
};
#endif //GYMNURE_MODELLOADER_H
