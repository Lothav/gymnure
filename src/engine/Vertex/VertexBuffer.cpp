#define TINYOBJLOADER_IMPLEMENTATION

#include <Util/Debug.hpp>
#include "tinyobjloader/tiny_obj_loader.h"

#include "VertexBuffer.h"

namespace Engine
{
    namespace Vertex
    {
        uint32_t VertexBuffer::getVertexSize() const
        {
            return vertex_count;
        }

        std::shared_ptr<Memory::Buffer> VertexBuffer::getBuffer() const
        {
            return buffer_;
        }

        std::vector<VertexData> VertexBuffer::createPrimitiveTriangle()
        {
            std::vector<VertexData> vertexBuffer =
                {
                    //       POSITION              UV              NORMAL
                    { {  1.0f,  1.0f, 0.0f }, {1.0f, 1.0f}, { 0.0f, 0.0f, -1.0f } },
                    { { -1.0f,  1.0f, 0.0f }, {1.0f, 1.0f}, { 0.0f, 0.0f, -1.0f } },
                    { {  0.0f, -1.0f, 0.0f }, {1.0f, 1.0f}, { 0.0f, 0.0f, -1.0f } }
                };

            initBuffer(vertexBuffer);

            return vertexBuffer;
        }

        std::vector<VertexData> VertexBuffer::loadObjModelVertices(const std::string &model_path, const std::string& obj_mtl)
        {
            std::vector <VertexData> vertex_data = {};

            auto assets_model_path = std::string(ASSETS_FOLDER_PATH_STR) + "/" + model_path;
            auto assets_obj_mtl    = std::string(ASSETS_FOLDER_PATH_STR) + "/" + obj_mtl;

            auto* obj_mtl_ptr = obj_mtl.empty() ? nullptr : assets_obj_mtl.c_str();

            tinyobj::attrib_t attrib;
            std::vector <tinyobj::shape_t> shapes;
            std::vector <tinyobj::material_t> materials;
            std::string err;

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

                    vertex_data.push_back(vertex);
                }
            }

            Debug::logInfo(assets_model_path + " object loaded! Vertex count: " +  std::to_string(vertex_data.size()));

            return vertex_data;
        }
    }
}