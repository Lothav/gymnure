//
// Created by tracksale on 8/31/17.
//

#ifndef OBSIDIAN2D_VERTEX_BUFFER_H
#define OBSIDIAN2D_VERTEX_BUFFER_H

#include <utility>
#include <vector>
#include <Util/Util.h>
#include "Memory/Memory.h"
#include "Memory/Buffer.h"
#include <memory>
#include <Descriptors/UniformBuffer.h>

namespace Engine
{
    namespace Vertex
    {
        class VertexBuffer
        {

        private:

            uint32_t vertex_count_ = 0;
            std::shared_ptr<Memory::Buffer<VertexData>> vertex_buffer_;

            uint32_t index_count_ = 0;
            std::shared_ptr<Memory::Buffer<uint32_t>> index_buffer_;

            void initBuffers(const std::vector<VertexData>& vertexData = {}, const std::vector<uint32_t>& indexBuffer = {});

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

            uint32_t getVertexCount() const;
            vk::Buffer getVertexBuffer() const;

            uint32_t getIndexCount() const;
            vk::Buffer getIndexBuffer() const;

            void loadObjModelVertices(const std::string& model_path, const std::string& obj_mtl);
            void createPrimitiveTriangle();

        };
    }
}
#endif //OBSIDIAN2D_VERTEX_BUFFER
