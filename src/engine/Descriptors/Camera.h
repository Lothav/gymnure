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

            vk::DescriptorBufferInfo vp_buffer_info_ {};
			std::unique_ptr<Memory::Buffer<glm::mat4>> vp_buffer_;

            vk::DescriptorBufferInfo pos_buffer_info_ {};
            std::unique_ptr<Memory::Buffer<glm::vec4>> pos_buffer_;

            glm::vec3 rotation = glm::vec3(0.0f);
			glm::vec3 center = glm::vec3(0.0f);

			float zoom_ = 10.f;
            float phi_ = 0.f;
            float theta_ = glm::radians(90.f);
            glm::vec4 pos = glm::vec4(0.f, 0.f, 0.f, 1.f);

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

			glm::vec3 getPosition() const
			{
            	return glm::vec3(view[3]);
            }

			void moveCamera(const glm::vec3& direction);
            void zoomCamera(float zoom);
            void rotateArcballCamera(float delta_phi, float delta_theta);
			void updateMVP();

            std::vector<vk::WriteDescriptorSet> getWrites(vk::DescriptorSet desc_set, uint32_t dst_bind, uint32_t pos_bind);
		};
	}
}

#endif //OBSIDIAN2D_CORE_UNIFORM_BUFFER_H
