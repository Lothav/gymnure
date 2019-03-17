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
#include <memory>

namespace Engine
{
    namespace Vertex
    {
        class VertexBuffer
        {

        private:

            uint32_t vertex_count = 0;
            std::shared_ptr<Memory::Buffer> buffer_;

            void initBuffer(const std::vector<VertexData>& vertexData)
            {
                vertex_count = static_cast<uint32_t>(vertexData.size());

                struct BufferData vbData = {};
                vbData.usage      = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
                vbData.properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
                vbData.size       = vertex_count * sizeof(VertexData);

                buffer_ = std::make_unique<Memory::Buffer>(vbData);
                Memory::Memory::copyMemory(buffer_->mem, vertexData.data(), vbData.size);
            }

        public:


            VertexBuffer() = default;
            ~VertexBuffer() = default;

            void* operator new(std::size_t size)
            {
                return mem::Provider::getMemory(size);
            }

            void operator delete(void* ptr)
            {
                // Do not free memory here!
            }

            std::shared_ptr<Memory::Buffer> getBuffer() const;
            uint32_t getVertexSize() const;
            std::vector<VertexData> loadObjModelVertices(const std::string& model_path, const std::string& obj_mtl);
            std::vector<VertexData> createPrimitiveTriangle();

        };
    }
}
#endif //OBSIDIAN2D_VERTEX_BUFFER
