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
		class UniformBuffer : public Memory::Buffer
		{
		public:

			UniformBuffer(struct BufferData uniformBufferData) : Buffer(uniformBufferData)
			{
				zoom 		= -7.0f;
				rotation 	= { 0.0f,  0.0f, 0.0f };
				cameraPos 	= { 0.0f,  0.0f, 0.0f };
			}

			~UniformBuffer() {}

		private:

			struct  {
				glm::mat4 model;
				glm::vec4 view;
				glm::mat4 projection;
			} mvp;

			glm::vec3 cameraPos = glm::vec3();
			glm::vec3 rotation = glm::vec3();
			float zoom = 0;

		public:

			void initModelView(u_int32_t width, u_int32_t height)
			{
				this->mvp.projection = glm::perspective(glm::radians(60.0f), (float)width / (float)height, 0.001f, 256.0f);;
				this->updateUniform();
			}

			void zoomCamera(double _zoom)
			{
				this->zoom += _zoom;
				if (this->zoom > -2) this->zoom = -2;

				std::cout << "zoom: " << this->zoom << std::endl;
				this->updateUniform();
			}

			void rotateWorld(const glm::vec3& _rotation)
			{
				this->rotation.x += _rotation.x;
				this->rotation.y += _rotation.y;
				this->updateUniform();
			}

			void updateUniform()
			{
				glm::mat4 viewMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, zoom));

				this->mvp.model = viewMatrix * glm::translate(glm::mat4(1.0), cameraPos);
				this->mvp.model = glm::rotate(this->mvp.model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
				this->mvp.model = glm::rotate(this->mvp.model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
				this->mvp.model = glm::rotate(this->mvp.model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

				this->mvp.view = glm::vec4(0.0f, 0.0f, -zoom, 0.0f);

				this->updateMVP();
			}

			void updateMVP()
			{
				VkResult res;
				Memory::Memory::copyMemory(_instance_device, this->mem, &this->mvp, sizeof(this->mvp));
			}

		};
	}
}


#endif //OBSIDIAN2D_CORE_UNIFORM_BUFFER_H
