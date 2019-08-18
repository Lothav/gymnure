#include "Queue.h"
namespace Engine
{
    namespace RenderPass
    {
        vk::Queue Queue::graphics_queue_{};
        vk::Queue Queue::present_queue_{};

        uint32_t  Queue::graphics_queue_index_ = UINT32_MAX;
        uint32_t  Queue::present_queue_index_ = UINT32_MAX;

        vk::Queue Queue::GetGraphicQueue()
        {
            return graphics_queue_;
        }

        vk::Queue Queue::GetPresentQueue()
        {
            return present_queue_;
        }

        uint32_t Queue::GetPresentQueueIndex()
        {
            return graphics_queue_index_;
        }

        uint32_t Queue::GetGraphicQueueIndex()
        {
            return graphics_queue_index_;
        }

        void Queue::LoadQueues()
        {
            auto app_data = ApplicationData::data;

            uint32_t graphics_queue_family_index_ = UINT32_MAX;
            uint32_t present_queue_family_index_ = UINT32_MAX;

            {
                std::vector<vk::Bool32> supports_present = {};
                supports_present.resize(app_data->queue_family_count);

                for (uint32_t i = 0; i < app_data->queue_family_count; i++) {
                    supports_present[i] = app_data->gpu.getSurfaceSupportKHR(i, app_data->surface);
                }

                // Search for a graphics and a present queue in the array of queue
                // families, try to find one that supports both
                for (uint32_t i = 0; i < app_data->queue_family_count; ++i) {
                    if ((app_data->queue_family_props[i].queueFlags & vk::QueueFlagBits::eGraphics)) {
                        if (graphics_queue_family_index_ == UINT32_MAX) {
                            graphics_queue_family_index_ = i;
                        }
                        if (supports_present[i] == VK_TRUE) {
                            graphics_queue_family_index_ = i;
                            present_queue_family_index_ = i;
                            break;
                        }
                    }
                }

                if (present_queue_family_index_ == UINT32_MAX) {
                    // If didn't find a queue that supports both graphics and present, then
                    // find a separate present queue.
                    for (size_t i = 0; i < app_data->queue_family_count; ++i){
                        if (supports_present[i] == VK_TRUE) {
                            present_queue_family_index_ = (uint32_t) i;
                            break;
                        }
                    }
                }
            }

            graphics_queue_index_ = graphics_queue_family_index_;
            present_queue_index_ = present_queue_family_index_;

            // Generate error if could not find queues that support graphics
            // and present
            if (graphics_queue_family_index_ == UINT32_MAX || present_queue_family_index_ == UINT32_MAX) {
                Debug::logErrorAndDie("Could not find a queues for both graphics and present");
            }

            graphics_queue_ = app_data->device.getQueue(graphics_queue_family_index_, 0);
            if (graphics_queue_family_index_ == present_queue_family_index_) {
                present_queue_ = graphics_queue_;
            } else {
                present_queue_ = app_data->device.getQueue(present_queue_family_index_, 0);
            }

            // @TODO set an proper transfer queue
            app_data->transfer_queue = graphics_queue_;
        }
    }
}