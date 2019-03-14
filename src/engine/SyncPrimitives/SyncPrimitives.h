//
// Created by luizorv on 9/7/17.
//

#ifndef OBSIDIAN2D_SYNCPRIMITIVES_H
#define OBSIDIAN2D_SYNCPRIMITIVES_H

#include <cassert>
#include <vector>
#include <zconf.h>
#include <memancpp/Provider.hpp>
#include "vulkan/vulkan.h"

namespace Engine
{
	namespace SyncPrimitives
	{
		class SyncPrimitives
		{
		private:

			VkDevice 						_instance_device;
			std::vector<VkFence> 			_fences = {};

		public:

			VkSemaphore  					imageAcquiredSemaphore;
			VkSemaphore 					renderSemaphore;

			SyncPrimitives(VkDevice device)
			{
				_instance_device = device;
			}

			~SyncPrimitives()
			{
				vkDestroySemaphore(_instance_device, imageAcquiredSemaphore, nullptr);
				vkDestroySemaphore(_instance_device, renderSemaphore, nullptr);
				for (auto &_fence : _fences) {
					vkDestroyFence(_instance_device, _fence, nullptr);
				}
			}

			void* operator new(std::size_t size)
			{
				return mem::Provider::getMemory(size);
			}

			void operator delete(void* ptr)
			{
				// Do not free memory here!
			}

			void createSemaphore()
			{
				VkSemaphoreCreateInfo imageAcquiredSemaphoreCreateInfo = {};
				imageAcquiredSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
				imageAcquiredSemaphoreCreateInfo.pNext = nullptr;
				imageAcquiredSemaphoreCreateInfo.flags = 0;

				VkResult res = vkCreateSemaphore(_instance_device, &imageAcquiredSemaphoreCreateInfo, nullptr, &imageAcquiredSemaphore);
				assert(res == VK_SUCCESS);

				res = vkCreateSemaphore(_instance_device, &imageAcquiredSemaphoreCreateInfo, nullptr, &renderSemaphore);
				assert(res == VK_SUCCESS);
			}

			void createFence(uint32_t size)
			{
				_fences.resize(size);

				VkFenceCreateInfo fenceInfo = {};
				fenceInfo.sType 			= VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
				fenceInfo.pNext 			= nullptr;
				fenceInfo.flags 			= VK_FENCE_CREATE_SIGNALED_BIT;

				for(u_int32_t i = 0; i< size; i++)
				{
					vkCreateFence(_instance_device, &fenceInfo, nullptr, &_fences[i]);
				}

			}

			VkFence* getFence(u_int32_t i)
			{
				return &this->_fences[i];
			}
		};
	}
}

#endif //OBSIDIAN2D_SYNCPRIMITIVES_H
