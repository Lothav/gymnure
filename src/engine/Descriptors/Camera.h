#ifndef OBSIDIAN2D_CORE_UNIFORM_BUFFER_H
#define OBSIDIAN2D_CORE_UNIFORM_BUFFER_H

#include <array>
#include "Util/Layers.h"
#include "Memory/Memory.h"
#include "Memory/Buffer.h"

namespace Engine
{
	namespace Descriptors
	{
		class Camera
		{

		private:

			std::unique_ptr<Memory::Buffer<glm::mat4>> buffer_;

            glm::vec3 rotation = glm::vec3(0.0f);
			glm::vec3 pos = glm::vec3(0, 0, -10.0f);
			glm::vec3 center = glm::vec3(0.0f);
			vk::DescriptorBufferInfo buffer_info_ {};

		public:

			glm::mat4 view{};
			glm::mat4 projection{};

            explicit Camera(u_int32_t width, u_int32_t height)
			{
                struct BufferData buffer_data = {};
                buffer_data.usage      = vk::BufferUsageFlagBits::eUniformBuffer;
                buffer_data.properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
                buffer_data.count      = 1;
            	buffer_  = std::make_unique<Memory::Buffer<glm::mat4>>(buffer_data);

                projection = glm::perspective(glm::radians(60.0f), (float)width / (float)height, 0.001f, 1000.0f);
                view = glm::lookAt(pos, center, glm::vec3(0, -1, 0));
                updateMVP();

				buffer_info_.offset = 0;
				buffer_info_.range  = VK_WHOLE_SIZE;
				buffer_info_.buffer = buffer_->getBuffer();
            }

            void* operator new(std::size_t size)
			{
				return mem::Provider::getMemory(size);
			}

			void operator delete(void* ptr)
			{
				// Do not free memory here!
			}

			void moveCamera(const glm::vec3& direction)
			{
				pos += direction;
				view = glm::lookAt(pos, center, glm::vec3(0, -1, 0));
				updateMVP();
			}

            void zoomCamera(float zoom)
            {
                auto factor = zoom * 0.2f + 1.0f;

                view[3][0] *= factor;
                view[3][1] *= factor;
                view[3][2] *= factor;

                updateMVP();
            }

			void rotateCamera(const glm::vec3& axis)
			{
				this->view = glm::rotate(this->view, 0.05f, axis);
				updateMVP();
			}

			void updateMVP()
			{
				buffer_->updateBuffer({projection * view});
			}

			vk::WriteDescriptorSet getWrite(vk::DescriptorSet desc_set, uint32_t dst_bind)
			{
                vk::WriteDescriptorSet write = {};
                write.pNext 			= nullptr;
                write.dstSet 			= desc_set;
                write.descriptorCount 	= 1;
                write.descriptorType 	= vk::DescriptorType::eUniformBuffer;
                write.pBufferInfo 		= &buffer_info_;
                write.dstBinding 		= dst_bind;

                return write;
			}

		};
	}
}

#endif //OBSIDIAN2D_CORE_UNIFORM_BUFFER_H
