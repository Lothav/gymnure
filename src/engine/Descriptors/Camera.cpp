#include "Camera.h"

namespace Engine
{
    namespace Descriptors
    {
        Camera::Camera(u_int32_t width, u_int32_t height)
        {
            struct BufferData buffer_data = {};

            projection = glm::perspective(glm::radians(40.0f), (float)width / (float)height, 0.001f, 1000.0f);
            view = glm::lookAt(glm::vec3(0.f, 0.f, zoom_), center, glm::vec3(0, -1, 0));

            buffer_data.usage      = vk::BufferUsageFlagBits::eUniformBuffer;
            buffer_data.properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
            buffer_data.count      = 1;
            vp_buffer_  = std::make_unique<Memory::Buffer<glm::mat4>>(buffer_data);
            vp_buffer_->updateBuffer({projection * view});

            vp_buffer_info_.offset = 0;
            vp_buffer_info_.range  = VK_WHOLE_SIZE;
            vp_buffer_info_.buffer = vp_buffer_->getBuffer();

            buffer_data.count      = 2;
            pos_buffer_ = std::make_unique<Memory::Buffer<glm::vec4>>(buffer_data);
            pos_buffer_->updateBuffer({ glm::vec4(0, 10, 0, 1), glm::vec4(0, 0, 0, 1)});

            pos_buffer_info_.offset = 0;
            pos_buffer_info_.range  = VK_WHOLE_SIZE;
            pos_buffer_info_.buffer = pos_buffer_->getBuffer();
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
            auto spherical_pos = glm::vec3(x, y, z);

            glm::vec3 center = glm::vec3(0.f, 0.f, 0.f);
            glm::vec3 right  = glm::cross(spherical_pos, glm::vec3(0, 1, 0));

            glm::vec3 up = glm::cross(spherical_pos, right);
            view = glm::lookAt(zoom_ * spherical_pos, center, up);
            pos = glm::vec4(spherical_pos, 1.f);

            updateMVP();
        }

        void Camera::updateMVP()
        {
            vp_buffer_->updateBuffer({projection * view});
            pos_buffer_->updateBuffer({glm::vec4(0, 10, 0, 1.f), pos});
        }

        std::vector<vk::WriteDescriptorSet> Camera::getWrites(vk::DescriptorSet desc_set, uint32_t vp_bind, uint32_t cam_pos_bind)
        {
            std::vector<vk::WriteDescriptorSet> writes = {};

            vk::WriteDescriptorSet write = {};

            write.pNext 			= nullptr;
            write.dstSet 			= desc_set;
            write.descriptorCount 	= 1;
            write.descriptorType 	= vk::DescriptorType::eUniformBuffer;
            write.pBufferInfo 		= &vp_buffer_info_;
            write.dstBinding 		= vp_bind;
            writes.push_back(write);

            write.pNext 			= nullptr;
            write.dstSet 			= desc_set;
            write.descriptorCount 	= 1;
            write.descriptorType 	= vk::DescriptorType::eUniformBuffer;
            write.pBufferInfo 		= &pos_buffer_info_;
            write.dstBinding 		= cam_pos_bind;
            writes.push_back(write);

            return writes;
        }
    }
}
