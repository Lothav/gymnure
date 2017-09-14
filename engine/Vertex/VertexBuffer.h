//
// Created by tracksale on 8/31/17.
//

#ifndef OBSIDIAN2D_VERTEX_BUFFER_H
#define OBSIDIAN2D_VERTEX_BUFFER_H

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

        };
    }
}
#endif //OBSIDIAN2D_VERTEX_BUFFER
