//
// Created by luizorv on 9/7/17.
//

#ifndef OBSIDIAN2D_SYNCPRIMITIVES_H
#define OBSIDIAN2D_SYNCPRIMITIVES_H

#include <vulkan/vulkan.hpp>
#include <cassert>
#include <vector>
#include <zconf.h>
#include <memancpp/Provider.hpp>
#include <ApplicationData.hpp>

namespace Engine
{
	namespace SyncPrimitives
	{
		class SyncPrimitives
		{
		private:

			std::vector<vk::Fence> fences_ = {};

		public:

			vk::Semaphore imageAcquiredSemaphore;
			vk::Semaphore renderSemaphore;

			SyncPrimitives() = default;
			~SyncPrimitives();

            void* operator new(std::size_t size)
			{
				return mem::Provider::getMemory(size);
			}

			void operator delete(void* ptr)
			{
				// Do not free memory here!
			}

			void createSemaphore();
			void createFences(uint32_t size);

			vk::Fence getFence(u_int32_t i);
		};
	}
}

#endif //OBSIDIAN2D_SYNCPRIMITIVES_H
