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
