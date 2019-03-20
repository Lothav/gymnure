#include "SyncPrimitives.h"

namespace Engine
{
    namespace SyncPrimitives
    {
        SyncPrimitives::~SyncPrimitives()
        {
            auto device = ApplicationData::data->device;

            device.destroySemaphore(imageAcquiredSemaphore, nullptr);
            device.destroySemaphore(renderSemaphore, nullptr);
            for (auto &fence : fences_) {
                device.destroyFence(fence, nullptr);
            }
        }

        void SyncPrimitives::createSemaphore()
        {
            auto device = ApplicationData::data->device;
            vk::Result res;

            vk::SemaphoreCreateInfo imageAcquiredSemaphoreCreateInfo = {};

            res = device.createSemaphore(&imageAcquiredSemaphoreCreateInfo, nullptr, &imageAcquiredSemaphore);
            assert(res == vk::Result::eSuccess);

            res = device.createSemaphore(&imageAcquiredSemaphoreCreateInfo, nullptr, &renderSemaphore);
            assert(res == vk::Result::eSuccess);
        }


        void SyncPrimitives::createFences(uint32_t size)
        {
            fences_.resize(size);

            auto device = ApplicationData::data->device;

            vk::FenceCreateInfo fenceInfo = {};
            fenceInfo.pNext = nullptr;
            fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;

            for(u_int32_t i = 0; i< size; i++)
                device.createFence(&fenceInfo, nullptr, &fences_[i]);
        }


        vk::Fence SyncPrimitives::getFence(u_int32_t i)
        {
            return this->fences_[i];
        }

    }

}

