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
                Memory::Memory::copyMemory(mem, _vertexData.data(), (_vertexData.size() * sizeof(VertexData)));
            }

        public:

            VertexBuffer(struct BufferData buffer_data, const std::vector<VertexData>& vertexData) : Buffer(buffer_data)
            {
                _vertexData = vertexData;
                updateMemoryWithData();
            }

            ~VertexBuffer() = default;

            void* operator new(std::size_t size)
            {
                return mem::Provider::getMemory(size);
            }

            void operator delete(void* ptr)
            {
                // Do not free memory here!
            }

            unsigned long getVertexSize() const
            {
                return _vertexData.size();
            }

            static std::vector<VertexData> loadModelVertices(const std::string& model_path, const char * obj_mtl = nullptr);

        };
    }
}
#endif //OBSIDIAN2D_VERTEX_BUFFER
