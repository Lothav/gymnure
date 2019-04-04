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

			float theta = 0.f;
            float phi = 0.f;

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
                if(direction.y != 0.f) {
                    glm::vec3 front = direction.y * glm::normalize(view[2]);
                    view = glm::translate(view, front);
                }

                if(direction.x != 0.f) {
                    glm::vec3 right = direction.x * glm::normalize(view[0]);
                    view = glm::translate(view, right);
                }

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

            glm::quat rotateBetweenVectors(glm::vec3 start, glm::vec3 dest)
            {
                start = glm::normalize(start);
                dest = glm::normalize(dest);

                float cosTheta = glm::dot(start, dest);
                glm::vec3 rotationAxis;

                if (cosTheta < -1 + 0.001f){
                    // special case when vectors in opposite directions:
                    // there is no "ideal" rotation axis
                    // So guess one; any will do as long as it's perpendicular to start
                    rotationAxis = cross(glm::vec3(0.0f, 0.0f, 1.0f), start);
                    if (glm::length2(rotationAxis) < 0.01 ) // bad luck, they were parallel, try again!
                        rotationAxis = cross(glm::vec3(1.0f, 0.0f, 0.0f), start);

                    rotationAxis = normalize(rotationAxis);
                    return glm::angleAxis(glm::radians(180.0f), rotationAxis);
                }

                rotationAxis = glm::cross(start, dest);

                float s = glm::sqrt( (1+cosTheta)*2 );
                float invs = 1 / s;

                return glm::quat(s * 0.5f, rotationAxis.x * invs, rotationAxis.y * invs, rotationAxis.z * invs);
            }

            void rotateCamera3(float delta_phi, float delta_theta)
            {
                glm::vec3 right = glm::normalize(view[0]);
                glm::vec3 up    = glm::normalize(view[1]);

                glm::vec3 p1 = glm::normalize(view[3]);
                glm::vec3 p2 = glm::normalize(p1 + 5 * (/*right*delta_phi - */up*delta_theta));

                std::cout << glm::to_string(p1) << " " << glm::to_string(p2) << std::endl;
                view = glm::toMat4(rotateBetweenVectors(p1, p2)) * view;

                std::cout << glm::to_string(view) << std::endl;

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
