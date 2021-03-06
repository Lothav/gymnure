//
// Created by luizorv on 9/7/17.
//

#ifndef OBSIDIAN2D_SYNCPRIMITIVES_H
#define OBSIDIAN2D_SYNCPRIMITIVES_H

#include <vulkan/vulkan.hpp>
#include <cassert>
#include <vector>
#include <ApplicationData.hpp>
#include <cstdint>

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

			void createSemaphore();
			void createFences(uint32_t size);

			vk::Fence getFence(uint32_t i);
		};
	}
}

#endif //OBSIDIAN2D_SYNCPRIMITIVES_H
