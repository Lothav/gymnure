//
// Created by tracksale on 9/14/17.
//

#ifndef GYMNURE_EVENTS_H
#define GYMNURE_EVENTS_H

#include <Descriptors/UniformBuffer.h>
#include "Util.h"


struct {
    bool left = false;
    bool right = false;
    bool middle = false;
} mouseButtons;


namespace Engine
{
    namespace Util
    {
        class Events : public Util
        {

        private :

            glm::vec3 rotation = glm::vec3();
            glm::vec3 cameraPos = glm::vec3();
            glm::vec2 mousePos;

        public:

            Events() = default;

            ~Events() = default;

            WindowEvent handleEvent(const xcb_generic_event_t *event, std::vector<Engine::Descriptors::DescriptorSet*> descSets) {

                switch (event->response_type & 0x7f)
                {
                    case XCB_CLIENT_MESSAGE: {
                        return WindowEvent::Close;
                    }

                    case XCB_MOTION_NOTIFY:
                    {
                        auto *motion = (xcb_motion_notify_event_t *) event;
                        for(uint16_t i = 0; i < descSets.size(); i++){
                            if (mouseButtons.left) {
                                descSets[i]->getUniformBuffer()->rotateWorld(
                                        glm::vec3( (mousePos.y - (float) motion->event_y) * 0.1f,
                                                   -(mousePos.x - (float) motion->event_x) * 0.1f, 0.0f)
                                );
                            }

                            if (mouseButtons.middle) {
                                descSets[i]->getUniformBuffer()->zoomCamera(-0.1);
                            }

                            if (mouseButtons.right) {
                                descSets[i]->getUniformBuffer()->zoomCamera(0.1);
                            }
                        }

                        mousePos = glm::vec2((float) motion->event_x, (float) motion->event_y);

                        return WindowEvent::None;
                    }

                    case XCB_BUTTON_PRESS: {
                        auto *press = (xcb_button_press_event_t *) event;
                        if (press->detail == XCB_BUTTON_INDEX_1)
                            mouseButtons.left = true;
                        if (press->detail == XCB_BUTTON_INDEX_2)
                            mouseButtons.middle = true;
                        if (press->detail == XCB_BUTTON_INDEX_3)
                            mouseButtons.right = true;

                        return WindowEvent::None;
                    }

                    case XCB_BUTTON_RELEASE: {
                        auto *press = (xcb_button_press_event_t *) event;
                        if (press->detail == XCB_BUTTON_INDEX_1)
                            mouseButtons.left = false;
                        if (press->detail == XCB_BUTTON_INDEX_2)
                            mouseButtons.middle = false;
                        if (press->detail == XCB_BUTTON_INDEX_3)
                            mouseButtons.right = false;

                        return WindowEvent::None;
                    }

                    case XCB_KEY_PRESS: {

                        auto *keyEvent = (const xcb_key_press_event_t *) event;
                        switch (keyEvent->detail) {
                            case 9:
                                return WindowEvent::Close;
                            default:
                                std::cout << "NONE pressed!" << std::endl;
                        }

                        return WindowEvent::None;
                    }

                    default:
                        return WindowEvent::None;
                }
            }
        };
    }
}
#endif //GYMNURE_EVENTS_H
