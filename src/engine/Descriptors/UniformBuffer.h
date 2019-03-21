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

			struct MVP{
				glm::mat4 model;
				glm::mat4 view;
				glm::mat4 projection;
			} mvp{};

            explicit UniformBuffer(const struct BufferData &uniformBufferData) : Buffer(uniformBufferData)
			{
				zoom 	 = -7.0f;
				rotation = { 0.0f,  0.0f, 0.0f };
			}

            void* operator new(std::size_t size)
			{
				return mem::Provider::getMemory(size);
			}

			void operator delete(void* ptr)
			{
				// Do not free memory here!
			}

		private:

			glm::vec3 rotation  = glm::vec3();
			float zoom = 0;

		public:

			void initModelView(u_int32_t width, u_int32_t height)
			{
				this->mvp.projection = glm::perspective(glm::radians(60.0f), (float)width / (float)height, 0.001f, 1000.0f);
				this->updateUniform();
			}

			void zoomCamera(double _zoom)
			{
				this->zoom += _zoom;
				if (this->zoom > -2) this->zoom = -2;

				std::cout << "Camera Zoom: " << this->zoom << std::endl;
				this->updateUniform();
			}

			void updateUniform()
			{
				this->mvp.model = glm::mat4x4(1.0f);
				this->mvp.view  = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, zoom));

				this->updateMVP();
			}

			void updateMVP()
			{
				Memory::Memory::copyMemory(this->mem, &this->mvp, sizeof(this->mvp));
			}

		};
	}
}


#endif //OBSIDIAN2D_CORE_UNIFORM_BUFFER_H
