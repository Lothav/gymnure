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
            return vertex_count_;
        }

        vk::Buffer VertexBuffer::getVertexBuffer() const
        {
            return vertex_buffer_->getBuffer();
        }

        uint32_t VertexBuffer::getIndexSize() const
        {
            return index_count_;
        }

        vk::Buffer VertexBuffer::getIndexBuffer() const
        {
            return index_buffer_->getBuffer();
        }

        void VertexBuffer::initBuffers(const std::vector<VertexData>& vertexData, const std::vector<uint32_t>& indexBuffer)
        {
            vertex_count_ = static_cast<uint32_t>(vertexData.size());

            struct BufferData buffer_data = {};
            buffer_data.properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
            buffer_data.usage      = vk::BufferUsageFlagBits::eVertexBuffer;
            buffer_data.size       = vertexData.size();

            vertex_buffer_ = std::make_unique<Memory::Buffer<VertexData>>(buffer_data);
            vertex_buffer_->updateBuffer(vertexData);

            if (!indexBuffer.empty()) {

                index_count_ = static_cast<uint32_t>(indexBuffer.size());

                buffer_data.usage = vk::BufferUsageFlagBits::eIndexBuffer;
                buffer_data.size  = indexBuffer.size();

                index_buffer_ = std::make_unique<Memory::Buffer<uint32_t>>(buffer_data);
                index_buffer_->updateBuffer(indexBuffer);
            }
        }

        void VertexBuffer::createPrimitiveTriangle(Descriptors::UniformBuffer* uo)
        {
            std::vector<VertexData> vertexBuffer =
            {
                //     POSITION              UV              NORMAL
                { { 1.0f,  1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f} },
                { {-1.0f,  1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f} },
                { { 0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f} }
            };

            // Setup indices
            std::vector<uint32_t> indexBuffer = { 0, 1, 2 };

            this->initBuffers(vertexBuffer, indexBuffer);
        }

        void VertexBuffer::loadObjModelVertices(const std::string &model_path, const std::string& obj_mtl)
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

            //@TODO INIT BUFFERS

            Debug::logInfo(assets_model_path + " object loaded! Vertex count: " +  std::to_string(vertex_data.size()));
        }
    }
}