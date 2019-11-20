#ifndef OBSIDIAN2D_CORE_UNIFORM_BUFFER_H
#define OBSIDIAN2D_CORE_UNIFORM_BUFFER_H

#include <array>
#include "Memory/Buffer.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "glm/ext.hpp"

namespace Engine::Descriptors
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

        explicit Camera(uint32_t width, uint32_t height);

        void moveCamera(const glm::vec3& direction);
        void zoomCamera(float zoom);
        void rotateArcballCamera(float delta_phi, float delta_theta);
        void updateMVP();

        std::vector<vk::WriteDescriptorSet> getWrites(vk::DescriptorSet desc_set, uint32_t vp_bind, uint32_t cam_pos_bind);
    };
}

#endif //OBSIDIAN2D_CORE_UNIFORM_BUFFER_H
