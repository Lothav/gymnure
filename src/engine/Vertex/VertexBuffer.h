#ifndef GYMNURE_VERTEX_BUFFER_H
#define GYMNURE_VERTEX_BUFFER_H

#include <memory>
#include <utility>
#include <vector>
#include <unordered_map>
#include "Memory/Memory.h"
#include "Memory/Buffer.h"
#include <Descriptors/Camera.h>
#include <Util/ModelDataLoader.h>
#include "tinyobjloader/tiny_obj_loader.h"

namespace Engine::Vertex
{
    template <class T, class U>
    class VertexBuffer
    {

    private:

        uint32_t vertex_count_ = 0;
        std::shared_ptr<Memory::Buffer<T>> vertex_buffer_;

        uint32_t index_count_ = 0;
        std::shared_ptr<Memory::Buffer<U>> index_buffer_;

    public:

        VertexBuffer() = default;
        ~VertexBuffer() = default;

        [[nodiscard]] uint32_t getVertexCount() const
        {
            return vertex_count_;
        }

        [[nodiscard]] vk::Buffer getVertexBuffer() const
        {
            return vertex_buffer_->getBuffer();
        }

        [[nodiscard]] uint32_t getIndexCount() const
        {
            return index_count_;
        }

        [[nodiscard]] vk::Buffer getIndexBuffer() const
        {
            return index_buffer_->getBuffer();
        }

        void initBuffers(const std::vector<T>& vertexData = {}, const std::vector<U>& indexBuffer = {})
        {
            vertex_count_ = static_cast<uint32_t>(vertexData.size());

            struct BufferData buffer_data = {};
            buffer_data.properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
            buffer_data.usage      = vk::BufferUsageFlagBits::eVertexBuffer;
            buffer_data.count      = vertexData.size();

            vertex_buffer_ = std::make_unique<Memory::Buffer<T>>(buffer_data);
            vertex_buffer_->updateBuffer(vertexData);

            if (!indexBuffer.empty()) {

                index_count_ = static_cast<uint32_t>(indexBuffer.size());

                buffer_data.usage = vk::BufferUsageFlagBits::eIndexBuffer;
                buffer_data.count = indexBuffer.size();

                index_buffer_ = std::make_unique<Memory::Buffer<U>>(buffer_data);
                index_buffer_->updateBuffer(indexBuffer);
            }
        }

        void loadObjModelVertices(const std::string& model_path, const std::string& obj_mtl)
        {
            std::vector <VertexData> vertex_data = {};
            std::vector <uint32_t> index_data = {};

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

            std::unordered_map<VertexData, uint32_t> uniqueVertices = {};

            for (const auto &shape : shapes)
            {
                for (const auto &index : shape.mesh.indices)
                {
                    glm::vec3 pos = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                    };

                    glm::vec2 uv = {
                        !attrib.texcoords.empty() ? attrib.texcoords[2 * index.texcoord_index + 0] : 1.0f,
                        !attrib.texcoords.empty() ? 1.0f - attrib.texcoords[2 * index.texcoord_index + 1] : 1.0f
                    };

                    glm::vec3 normal = {
                        index.normal_index > -1 ? attrib.normals[3 * index.normal_index + 0] : 1.0f,
                        index.normal_index > -1 ? attrib.normals[3 * index.normal_index + 1] : 1.0f,
                        index.normal_index > -1 ? attrib.normals[3 * index.normal_index + 2] : 1.0f
                    };

                    struct VertexData vertex = { pos, uv, normal };

                    if (uniqueVertices.count(vertex) == 0) {
                        uniqueVertices[vertex] = static_cast<uint32_t>(vertex_data.size());
                        vertex_data.push_back(vertex);
                    }

                    index_data.push_back(uniqueVertices[vertex]);
                }
            }

            this->initBuffers(vertex_data, index_data);

            Debug::logInfo( assets_model_path + " object loaded! "
                "Vertex count: " +  std::to_string(vertex_data.size()) + " / " +
                "Index count: " +  std::to_string(index_data.size()) );
        }

        void createPrimitiveTriangle()
        {
            std::vector<VertexData> vertexBuffer =
            {
                //     POSITION              UV              NORMAL
                { { 1.0f,  1.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f} },
                { {-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f} },
                { { 0.0f, -1.0f, 0.0f}, {0.5f, 1.0f}, {0.0f, 0.0f, -1.0f} }
            };

            // Setup indices
            std::vector<uint32_t> indexBuffer = { 0, 1, 2 };

            this->initBuffers(vertexBuffer, indexBuffer);
        }

        void createPrimitiveQuad()
        {
            std::vector<VertexData> vertexBuffer =
            {
                //     POSITION              UV              NORMAL
                { { 10.0f, 0.0f,  10.0f}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f} },
                { { 10.0f, 0.0f, -10.0f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f} },
                { {-10.0f, 0.0f, -10.0f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f} },
                { {-10.0f, 0.0f,  10.0f}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f} },
            };

            // Setup indices
            std::vector<uint32_t> indexBuffer = { 0, 1, 2, 0, 2, 3 };

            this->initBuffers(vertexBuffer, indexBuffer);
        }

    };
}
#endif // GYMNURE_VERTEX_BUFFER
