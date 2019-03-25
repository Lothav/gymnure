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
		class UniformBuffer
		{

		private:

			std::unique_ptr<Memory::Buffer<glm::mat4>> buffer_;

		public:

			glm::mat4 model;
			glm::mat4 view;
			glm::mat4 projection;

            explicit UniformBuffer()
			{
                struct BufferData buffer_data = {};
                buffer_data.usage      = vk::BufferUsageFlagBits::eUniformBuffer;
                buffer_data.properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
                buffer_data.size       = 3;

            	buffer_  = std::make_unique<Memory::Buffer<glm::mat4>>(buffer_data);
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
				this->projection = glm::perspective(glm::radians(60.0f), (float)width / (float)height, 0.001f, 1000.0f);
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
				this->model = glm::mat4x4(1.0f);
				this->view  = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, zoom));

				this->updateMVP();
			}

			void updateMVP()
			{
				this->buffer_->updateBuffer({this->model, this->view, this->projection});
			}

			vk::WriteDescriptorSet getWrite(vk::DescriptorSet desc_set)
			{
                vk::DescriptorBufferInfo *buffer_info = new vk::DescriptorBufferInfo();
				buffer_info->offset 	= 0;
                buffer_info->range  	= buffer_->getSize();
                buffer_info->buffer 	= buffer_->getBuffer();

                vk::WriteDescriptorSet write = {};
                write.pNext 			= nullptr;
                write.dstSet 			= desc_set;
                write.descriptorCount 	= 1;
                write.descriptorType 	= vk::DescriptorType::eUniformBuffer;
                write.pBufferInfo 		= buffer_info;
				// Binds this uniform buffer to binding point 0
                write.dstBinding 		= 0;

                return write;
			}

		};
	}
}


#endif //OBSIDIAN2D_CORE_UNIFORM_BUFFER_H
