//
// Created by tracksale on 8/31/17.
//

#ifndef OBSIDIAN2D_VERTEX_BUFFER_H
#define OBSIDIAN2D_VERTEX_BUFFER_H

#include <memory>
#include <utility>
#include <vector>
#include <Util/Util.h>
#include "Memory/Memory.h"
#include "Memory/Buffer.h"
#include <Descriptors/Camera.h>
#include <Util/ModelDataLoader.h>

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

        public:

            VertexBuffer() = default;
            ~VertexBuffer() = default;

            void initBuffers(const std::vector<VertexData>& vertexData = {}, const std::vector<uint32_t>& indexBuffer = {});

            uint32_t getVertexCount() const;
            vk::Buffer getVertexBuffer() const;

            uint32_t getIndexCount() const;
            vk::Buffer getIndexBuffer() const;

            void loadObjModelVertices(const std::string& model_path, const std::string& obj_mtl);
            void createPrimitiveTriangle();
            void createPrimitiveQuad();

        };
    }
}
#endif //OBSIDIAN2D_VERTEX_BUFFER
