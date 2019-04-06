#include "Camera.h"

namespace Engine
{
    namespace Descriptors
    {
        Camera::Camera(u_int32_t width, u_int32_t height)
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

        void Camera::moveCamera(const glm::vec3& direction)
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

        void Camera::zoomCamera(float zoom)
        {
            auto factor = zoom * 0.2f + 1.0f;

            view[3][0] *= factor;
            view[3][1] *= factor;
            view[3][2] *= factor;

            updateMVP();
        }

        glm::quat Camera::rotateBetweenVectors(glm::vec3 start, glm::vec3 dest)
        {
            start = glm::normalize(start);
            dest = glm::normalize(dest);

            float cosTheta = glm::dot(start, dest);
            glm::vec3 rotationAxis;

            if (cosTheta < -1 + 0.001f){
                // special case when vectors in opposite directions:
                // there is no "ideal" rotation axis
                // So guess one; any will do as long as it's perpendicular to start
                rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), start);
                if (glm::length2(rotationAxis) < 0.01 ) // bad luck, they were parallel, try again!
                    rotationAxis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), start);

                rotationAxis = glm::normalize(rotationAxis);
                return glm::angleAxis(glm::radians(180.0f), rotationAxis);
            }

            rotationAxis = glm::cross(start, dest);

            float s = glm::sqrt( (1+cosTheta)*2 );
            float invs = 1 / s;

            return glm::quat(s * 0.5f, rotationAxis.x * invs, rotationAxis.y * invs, rotationAxis.z * invs);
        }

        void Camera::rotateArcballCamera(float delta_phi, float delta_theta)
        {
            glm::vec3 right = glm::normalize(view[0]);
            glm::vec3 up    = glm::normalize(view[1]);

            glm::vec3 p1 = glm::normalize(view[3]);
            glm::vec3 p2 = glm::normalize(p1 + 5 * (right*delta_phi - up*delta_theta));

            view = glm::toMat4(rotateBetweenVectors(p1, p2)) * view;

            updateMVP();
        }

        void Camera::updateMVP()
        {
            buffer_->updateBuffer({projection * view});
        }

        vk::WriteDescriptorSet Camera::getWrite(vk::DescriptorSet desc_set, uint32_t dst_bind)
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
    }
}
