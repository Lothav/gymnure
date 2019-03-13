#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobjloader/tiny_obj_loader.h"

#include "VertexBuffer.h"

namespace Engine
{
    namespace Vertex
    {
        std::vector<VertexData> VertexBuffer::loadModelVertices(const std::string &model_path, const std::string& obj_mtl)
        {
            std::vector <VertexData> vertexData = {};

            tinyobj::attrib_t attrib;
            std::vector <tinyobj::shape_t> shapes;
            std::vector <tinyobj::material_t> materials;
            std::string err;
            std::map<std::string, int> map;

            auto assets_model_path = std::string(ASSETS_FOLDER_PATH_STR) + "/" + model_path;
            auto assets_obj_mtl = std::string(ASSETS_FOLDER_PATH_STR) + "/" + obj_mtl;

            auto* obj_mtl_ptr = obj_mtl.empty() ? nullptr : assets_obj_mtl.c_str();

            if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, assets_model_path.c_str(), obj_mtl_ptr)) {
                throw std::runtime_error(err);
            }

            for (const auto &shape : shapes) {
                for (const auto &index : shape.mesh.indices) {
                    struct VertexData vertex =
                        {
                            {
                                attrib.vertices[3 * index.vertex_index + 0],
                                attrib.vertices[3 * index.vertex_index + 1],
                                attrib.vertices[3 * index.vertex_index + 2]
                            },
                            {
                                !attrib.texcoords.empty() ? attrib.texcoords[2 * index.texcoord_index + 0] : 1.0f,
                                !attrib.texcoords.empty() ? 1.0f - attrib.texcoords[2 * index.texcoord_index + 1] : 1.0f
                            },
                            {
                                index.normal_index > -1 ? attrib.normals[3 * index.normal_index + 0] : 1.0f,
                                index.normal_index > -1 ? attrib.normals[3 * index.normal_index + 1] : 1.0f,
                                index.normal_index > -1 ? attrib.normals[3 * index.normal_index + 2] : 1.0f
                            }

                        };

                    vertexData.push_back(vertex);
                }
            }

            return vertexData;
        }
    }
}