//
// Created by tracksale on 8/31/17.
//

#ifndef OBSIDIAN2D_VERTEX_BUFFER_H
#define OBSIDIAN2D_VERTEX_BUFFER_H

#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobjloader/tiny_obj_loader.h"

#include <vector>
#include <Util/Util.h>
#include "Memory/Memory.h"
#include "Memory/Buffer.h"

namespace Engine
{
    namespace Vertex
    {
        class VertexBuffer: public Memory::Buffer {

        private:
            std::vector<VertexData> _vertexData = {};

            void updateMemoryWithData()
            {
                Memory::Memory::copyMemory(_instance_device, mem, _vertexData.data(), (_vertexData.size() * sizeof(VertexData)));
            }

        public:

            VertexBuffer(struct BufferData buffer_data, std::vector<VertexData> vertexData): Buffer(buffer_data)
            {
                this->_vertexData = vertexData;
                updateMemoryWithData();
            }

            void update(std::vector<VertexData> vertex)
            {
                _vertexData = vertex;
                this->updateMemoryWithData();
            }

            unsigned long getVertexSize()
            {
                return _vertexData.size();
            }

            static std::vector<VkBuffer> getBuffersFromVector(std::vector<VertexBuffer *> vector)
            {
                std::vector<VkBuffer> buffers = {};

                for(auto vertex : vector){
                    buffers.push_back(vertex->buf);
                }

                return buffers;
            }


            static std::vector<VertexData> loadModelVertices(std::string model_path)
            {
                std::vector<VertexData> vertexData = {};

                tinyobj::attrib_t attrib;
                std::vector<tinyobj::shape_t> shapes;
                std::vector<tinyobj::material_t> materials;
                std::string err;

                if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, model_path.c_str())) {
                    throw std::runtime_error(err);
                }

                std::vector<VertexData> uniqueVertices = {};


                for (const auto& shape : shapes)
                {
                    for (const auto& index : shape.mesh.indices)
                    {
                        struct VertexData vertex =
                                {
                                        {
                                                attrib.vertices[3 * index.vertex_index + 0],
                                                attrib.vertices[3 * index.vertex_index + 1],
                                                attrib.vertices[3 * index.vertex_index + 2]
                                        },
                                        {
                                                attrib.texcoords[2 * index.texcoord_index + 0],
                                                1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                                        },
                                        {
                                                1.0f, 1.0f, 1.0f
                                        }

                                };

                        vertexData.push_back(vertex);
                    }
                }
                return vertexData;
            }

        };
    }
}
#endif //OBSIDIAN2D_VERTEX_BUFFER
