#ifndef OBSIDIAN2D_CORE_UNIFORM_BUFFER_H
#define OBSIDIAN2D_CORE_UNIFORM_BUFFER_H

#include <array>
#include "Util/Layers.h"
#include "Memory/Memory.h"
#include "Memory/Buffer.h"

typedef struct _view_camera {
	glm::vec3 eye;
	glm::vec3 center;
	glm::vec3 up;
} ViewCamera;

namespace Engine
{
	namespace Descriptors
	{
		class UniformBuffer : public Memory::Buffer
		{
		public:

			UniformBuffer(struct BufferData uniformBufferData) : Buffer(uniformBufferData)
			{}

			~UniformBuffer()
			{}

		private:

			ViewCamera _view_camera;

			glm::mat4 _projection;
			glm::mat4 _view;
			glm::mat4 _model;

			glm::mat4 _mvp; // Model View Projection

			const std::array<float, 3> _default_eye		= {0, 0, 1};
			const std::array<float, 3> _default_center	= {0, 0, 0};
			const std::array<float, 3> _default_up 		= {0, 1, 0};

			void updateMVP()
			{
				this->_mvp = this->_projection * this->_view * this->_model;
			}

		public:

			void initModelView(u_int32_t width, u_int32_t height)
			{
				float fov = glm::radians(45.0f);
				if (width > height) {
					fov *= static_cast<float>(height) / static_cast<float>(width);
				}
				this->_projection = glm::perspective(fov, static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);

				this->_view_camera.eye 	  =  glm::vec3(_default_eye[0], _default_eye[1], _default_eye[2]);
				this->_view_camera.center =  glm::vec3(_default_center[0], _default_center[1], _default_center[2]);
				this->_view_camera.up     =  glm::vec3(_default_up[0], _default_up[1], _default_up[2]);
				this->_view               =  glm::lookAt(this->_view_camera.eye, this->_view_camera.center, this->_view_camera.up);

				this->_model = this->_view * glm::translate(glm::mat4(), {0, 0, 1});

				this->updateMVP();
			}

			void setCameraViewEye(glm::vec3 eye)
			{
				this->_view_camera.eye = eye;
				this->_view = glm::lookAt( this->_view_camera.eye, this->_view_camera.center, this->_view_camera.up );
				this->updateMVP();
			}

			void setCameraViewCenter(glm::vec3 center)
			{
				this->_view_camera.center = center;
				this->_view = glm::lookAt( this->_view_camera.eye, this->_view_camera.center, this->_view_camera.up );
				this->updateMVP();
			}

			void setCameraViewUp(glm::vec3 up)
			{
				this->_view_camera.up = up;
				this->_view = glm::lookAt( this->_view_camera.eye, this->_view_camera.center, this->_view_camera.up );
				this->updateMVP();
			}

            void zoomCamera(glm::vec3 translate_vec)
            {
                this->_view = glm::scale(this->_view, translate_vec);
                this->updateMVP();
            }

			void rotateCamera(glm::vec3 rot)
            {
				this->_view = glm::rotate(this->_view, glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
				this->_view = glm::rotate(this->_view, glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
				this->_view = glm::rotate(this->_view, glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));

				this->updateMVP();
			}

			glm::mat4 getMVP()
			{
				return this->_mvp;
			}

			std::array<float, 3> getCameraDefaultEye()
			{
				return _default_eye;
			};

			std::array<float, 3> getCameraDefaultCenter()
			{
				return _default_center;
			};

			std::array<float, 3> getCameraDefaultUp()
			{
				return _default_up;
			};

			void updateMVP(VkDevice device)
			{
				VkResult res;
				Memory::Memory::copyMemory(device, this->mem, &this->_mvp, sizeof(this->_mvp));
			}

		};
	}
}


#endif //OBSIDIAN2D_CORE_UNIFORM_BUFFER_H
