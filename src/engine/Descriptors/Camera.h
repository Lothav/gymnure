#ifndef OBSIDIAN2D_CORE_UNIFORM_BUFFER_H
#define OBSIDIAN2D_CORE_UNIFORM_BUFFER_H

#include <array>
#include "Util/Layers.h"
#include "Memory/Memory.h"
#include "Memory/Buffer.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "glm/ext.hpp"

namespace Engine
{
	namespace Descriptors
	{
		class Camera
		{

		private:

			std::unique_ptr<Memory::Buffer<glm::mat4>> buffer_;

            glm::vec3 rotation = glm::vec3(0.0f);
			glm::vec3 pos = glm::vec3(10.f, 0, -10.f);
			glm::vec3 center = glm::vec3(0.0f);
			vk::DescriptorBufferInfo buffer_info_ {};

		public:

			glm::mat4 view{};
			glm::mat4 projection{};

            explicit Camera(u_int32_t width, u_int32_t height);

            void* operator new(std::size_t size)
			{
				return mem::Provider::getMemory(size);
			}

			void operator delete(void* ptr)
			{
				// Do not free memory here!
			}

			void moveCamera(const glm::vec3& direction);
            void zoomCamera(float zoom);
            void rotateArcballCamera(float delta_phi, float delta_theta);
			void updateMVP();

            glm::quat rotateBetweenVectors(glm::vec3 start, glm::vec3 dest);
			vk::WriteDescriptorSet getWrite(vk::DescriptorSet desc_set, uint32_t dst_bind);
		};
	}
}

#endif //OBSIDIAN2D_CORE_UNIFORM_BUFFER_H
