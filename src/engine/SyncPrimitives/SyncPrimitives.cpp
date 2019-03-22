#include "SyncPrimitives.h"
#include "Util/Debug.hpp"

namespace Engine
{
    namespace SyncPrimitives
    {
        SyncPrimitives::~SyncPrimitives()
        {
            auto device = ApplicationData::data->device;

            device.destroySemaphore(imageAcquiredSemaphore);
            device.destroySemaphore(renderSemaphore);
            for (auto &fence : fences_) {
                device.destroyFence(fence);
            }
        }

        void SyncPrimitives::createSemaphore()
        {
            auto device = ApplicationData::data->device;

            vk::SemaphoreCreateInfo imageAcquiredSemaphoreCreateInfo = {};
            imageAcquiredSemaphore = device.createSemaphore(imageAcquiredSemaphoreCreateInfo);
            renderSemaphore = device.createSemaphore(imageAcquiredSemaphoreCreateInfo);
        }


        void SyncPrimitives::createFences(uint32_t size)
        {
            fences_.resize(size);

            auto device = ApplicationData::data->device;

            vk::FenceCreateInfo fenceInfo = {};
            fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;

            for(u_int32_t i = 0; i < size; i++)
                DEBUG_CALL(fences_[i] = device.createFence(fenceInfo));
        }

        vk::Fence SyncPrimitives::getFence(u_int32_t i)
        {
            return this->fences_[i];
        }

    }

}

