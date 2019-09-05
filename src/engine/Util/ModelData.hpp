#ifndef GYMNURE_MODELDATA_HPP
#define GYMNURE_MODELDATA_HPP

#include <vector>
#include <memory>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace Engine
{
    struct VertexData
    {
        glm::vec3 pos;
        glm::vec2 uv;
        glm::vec3 normal;

        bool operator==(const VertexData &other) const {
            return pos == other.pos && uv == other.uv;
        }
    };

    struct Material
    {
        std::string texture_path;
    };

    struct Mesh
    {
        std::shared_ptr<std::vector<VertexData>> vertexData;
        std::shared_ptr<Material> material;
    };

    struct Model
    {
        std::shared_ptr<std::vector<std::shared_ptr<Mesh>>> meshes;
    };
}

template<> struct std::hash<Engine::VertexData>
{
    size_t operator()(Engine::VertexData const& vertex) const
    {
        return std::hash<glm::vec3>()(vertex.pos) ^ (std::hash<glm::vec2>()(vertex.uv) << 1);
    }
};

#endif //GYMNURE_MODELDATA_HPP
