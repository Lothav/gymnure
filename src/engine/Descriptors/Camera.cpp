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

            projection = glm::perspective(glm::radians(40.0f), (float)width / (float)height, 0.001f, 1000.0f);
            view = glm::lookAt(glm::vec3(0.f, 0.f, -zoom_), center, glm::vec3(0, -1, 0));
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
            zoom_ += zoom;

            if(zoom_ < 2.f)
                zoom_ = 2.f;

            view[3] = glm::normalize(view[3]) * zoom_;

            updateMVP();
        }

        void Camera::rotateArcballCamera(float delta_phi, float delta_theta)
        {
            theta_ += delta_theta;
            theta_ = glm::clamp(theta_, glm::radians(5.f), glm::radians(175.f));

            phi_ += delta_phi;

            float x = glm::sin(theta_) * glm::sin(phi_);
            float y = glm::cos(theta_);
            float z = glm::cos(phi_) * glm::sin(theta_);

            glm::vec3 pos    = glm::vec3(x, y, z);
            glm::vec3 center = glm::vec3(0.f, 0.f, 0.f);
            glm::vec3 right  = glm::cross(pos, glm::vec3(0, -1, 0));

            glm::vec3 up = glm::cross(pos, right);
            view = glm::lookAt(zoom_ * pos, center, up);

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
