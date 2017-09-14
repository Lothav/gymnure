//
// Created by luizorv on 9/7/17.
//

#ifndef OBSIDIAN2D_SYNCPRIMITIVES_H
#define OBSIDIAN2D_SYNCPRIMITIVES_H

#include <vector>
#include <assert.h>
#include "vulkan/vulkan.h"

namespace Gymnure
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
				u_int32_t i;
				for (i = 0; i < _fences.size(); i++)
				{
					vkDestroyFence(_instance_device, _fences[i], nullptr);
				}
				vkDestroySemaphore(_instance_device, imageAcquiredSemaphore, nullptr);
				vkDestroySemaphore(_instance_device, renderSemaphore, nullptr);
			}

			void createSemaphore()
			{
				VkSemaphoreCreateInfo imageAcquiredSemaphoreCreateInfo;
				imageAcquiredSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
				imageAcquiredSemaphoreCreateInfo.pNext = NULL;
				imageAcquiredSemaphoreCreateInfo.flags = 0;

				VkResult res = vkCreateSemaphore(_instance_device, &imageAcquiredSemaphoreCreateInfo, NULL, &imageAcquiredSemaphore);
				assert(res == VK_SUCCESS);

				res = vkCreateSemaphore(_instance_device, &imageAcquiredSemaphoreCreateInfo, NULL, &renderSemaphore);
				assert(res == VK_SUCCESS);
			}

			void createFence(uint32_t size)
			{

				_fences.resize(size);

				VkFenceCreateInfo fenceInfo;
				fenceInfo.sType 			= VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
				fenceInfo.pNext 			= NULL;
				fenceInfo.flags 			= 0;

				for(u_int32_t i = 0; i< size; i++)
				{
					vkCreateFence(_instance_device, &fenceInfo, NULL, &_fences[i]);
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
