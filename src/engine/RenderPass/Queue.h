#ifndef GYMNURE_QUEUE_H
#define GYMNURE_QUEUE_H

#include <ApplicationData.hpp>
#include <Util/Debug.hpp>

namespace Engine
{
    namespace RenderPass
    {
        class Queue
        {

        private:

            static vk::Queue graphics_queue_;
            static vk::Queue present_queue_;
            static uint32_t graphics_queue_index_;
            static uint32_t present_queue_index_;

        public:

            static void LoadQueues();
            static vk::Queue GetGraphicQueue();
            static vk::Queue GetPresentQueue();

            static uint32_t GetGraphicQueueIndex();
            static uint32_t GetPresentQueueIndex();
        };
    }
}
#endif //GYMNURE_QUEUE_H
