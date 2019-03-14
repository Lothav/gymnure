#include <utility>

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

            uint32_t vertex_count;

        public:

            VertexBuffer(struct BufferData buffer_data, std::vector<VertexData> vertexData) : Buffer(buffer_data)
            {
                vertex_count = static_cast<uint32_t>(vertexData.size());
                Memory::Memory::copyMemory(mem, vertexData.data(), (vertex_count * sizeof(VertexData)));
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

            uint32_t getVertexSize() const
            {
                return vertex_count;
            }

            static std::vector<VertexData> loadObjModelVertices(const std::string& model_path, const std::string& obj_mtl);
            static std::vector<VertexData> createPrimitiveTriangle();

        };
    }
}
#endif //OBSIDIAN2D_VERTEX_BUFFER
