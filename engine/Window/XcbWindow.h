#ifndef OBSIDIAN2D_RENDERER_XCBWINDOW_
#define OBSIDIAN2D_RENDERER_XCBWINDOW_

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <bitset>
#include <xcb/xcb.h>

#include "Window/Window.h"
#include "Obsidian2D/Core/WindowEvent.h"
#include <unistd.h>

namespace Gymnure
{
	namespace Window
	{
		class XcbWindow: public Window
		{
		public:

			~XcbWindow() {
				xcb_destroy_window(connection, window);
				xcb_disconnect(connection);
			}
		private:

			xcb_screen_t* 							screen;
			xcb_window_t 							window;
			xcb_connection_t*						connection;

			std::array<float, 3>						camera_eye;
			std::array<float, 3>						camera_center;
			std::array<float, 3>						camera_up;

			void setConnection()
			{
				const xcb_setup_t *setup;
				xcb_screen_iterator_t iter;
				int scr;

				connection = xcb_connect(NULL, &scr);
				if (connection == NULL || xcb_connection_has_error(connection)) {
					std::cout << "Unable to make an XCB connection\n";
					exit(-1);
				}

				setup = xcb_get_setup(connection);
				iter = xcb_setup_roots_iterator(setup);
				while (scr-- > 0) xcb_screen_next(&iter);

				screen = iter.data;
			}

			void createWindow()
			{
				assert(this->width > 0);
				assert(this->height > 0);

				uint32_t value_mask, value_list[32];

				window = xcb_generate_id(connection);

				value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
				value_list[0] = screen->black_pixel;
				value_list[1] = XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_BUTTON_PRESS |
								XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_LEAVE_WINDOW;

				xcb_create_window(connection, XCB_COPY_FROM_PARENT, window, screen->root,
								  0, 0, (uint16_t)this->width, (uint16_t)this->height, 0,
								  XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, value_mask, value_list);

				/* Magic code that will send notification when window is destroyed */
				xcb_intern_atom_cookie_t cookie = xcb_intern_atom(connection, 1, 12, "WM_PROTOCOLS");
				xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(connection, cookie, 0);

				xcb_intern_atom_cookie_t cookie2 = xcb_intern_atom(connection, 0, 16, "WM_DELETE_WINDOW");
				xcb_intern_atom_reply_t *atom_wm_delete_window = xcb_intern_atom_reply(connection, cookie2, 0);

				xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, (*reply).atom, 4, 32, 1,
									&(*atom_wm_delete_window).atom);
				free(reply);

				xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window,
									XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, strlen(APP_NAME), APP_NAME);

				xcb_flush(connection);

				xcb_map_window(connection, window);

				// Force the x/y coordinates to 100, 100 results are identical in consecutive
				// runs
				const uint32_t coords[] = {100, 100};
				xcb_configure_window(connection, window, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, coords);
				xcb_flush(connection);
			}

			void createSurface()
			{
				VkXcbSurfaceCreateInfoKHR createInfo = {};
				createInfo.sType 						= VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
				createInfo.pNext 						= nullptr;
				createInfo.connection 					= connection;
				createInfo.window 						= window;

				VkResult res = vkCreateXcbSurfaceKHR(instance, &createInfo, nullptr, &surface);

				assert(res == VK_SUCCESS);
			}

		public:
			XcbWindow(int32_t width, int32_t height)
			{
				this->width = width;
				this->height = height;
			}

			void bootstrap()
			{
				this->createApplication();
				this->setConnection();
				this->createWindow();
				this->createSurface();
				this->initGraphicPipeline();

				/* Fist Data */
				createCommandBuffer();
				pushTexture("../../include/Obsidian2D/Renderer/shaders/medivh.jpg");
				std::vector<VertexData> vertexData =
					{
						{ { -4.0f,  3.0f, 0.0f }, { 0.0f, 1.0f },{ 0.0f, 0.0f, 1.0f } },
						{ { -4.0f, -3.0f, 0.0f }, { 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } },
						{ {  4.0f,  3.0f, 0.0f }, { 1.0f, 1.0f },{ 0.0f, 0.0f, 1.0f } },

						{ {  4.0f,  3.0f, 0.0f }, { 1.0f, 1.0f },{ 0.0f, 0.0f, 1.0f } },
						{ { -4.0f, -3.0f, 0.0f }, { 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } },
						{ {  4.0f, -3.0f, 0.0f }, { 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } }
					};
				pushVertex(vertexData);
				recordCommandBuffer();

				/* Second Data */
				createCommandBuffer();
				pushTexture("../../include/Obsidian2D/Renderer/shaders/wahre.png");
				std::vector<VertexData> vertexData2 =
					{
						{ { -2.0f,  0.0f, 0.0f }, { 0.0f, 1.0f },{ 0.0f, 0.0f, 1.0f } },
						{ { -1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } },
						{ {  1.0f,  1.0f, 0.0f }, { 1.0f, 1.0f },{ 0.0f, 0.0f, 1.0f } },

						{ {  1.0f,  1.0f, 0.0f }, { 1.0f, 1.0f },{ 0.0f, 0.0f, 1.0f } },
						{ { -1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } },
						{ {  1.0f, -1.0f, 0.0f }, { 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } }
					};
				pushVertex(vertexData2);
				recordCommandBuffer();

				camera_center   = descriptor_set[descriptor_set.size()-1]->getUniformBuffer()->getCameraDefaultCenter();
				camera_eye      = descriptor_set[descriptor_set.size()-1]->getUniformBuffer()->getCameraDefaultEye();
				camera_up       = descriptor_set[descriptor_set.size()-1]->getUniformBuffer()->getCameraDefaultUp();
			}

			WindowEvent poolEvent()
			{
				xcb_generic_event_t* e = nullptr;
				//xcb_intern_atom_reply_t *atom_wm_delete_window = nullptr;

				while ((e = xcb_poll_for_event(connection))) {
					if ((e->response_type & ~0x80) == XCB_CLIENT_MESSAGE) {
						//if((*(xcb_client_message_event_t*)e).data.data32[0] == (*atom_wm_delete_window).atom) {
							return WindowEvent::Close;
						//}
					} else if((e->response_type & ~0x80) == XCB_ENTER_NOTIFY) {
						return WindowEvent::Focus;
					} else if((e->response_type & ~0x80) == XCB_LEAVE_NOTIFY) {
						return WindowEvent::Blur;
					} else if((e->response_type & ~0x80) == XCB_BUTTON_PRESS) {
						return WindowEvent::Click;
					} else if((e->response_type & ~0x80) == XCB_BUTTON_RELEASE) {
						//return WindowEvent::ClickEnd;
					} else if((e->response_type & ~0x80) == XCB_KEY_PRESS) {

						xcb_key_press_event_t * kp = (xcb_key_press_event_t *)e;

						if( kp->detail == 'O'){
							camera_center[0]-= 0.1f;
						}else if(kp->detail == 'P'){
							camera_center[1] += 0.1f;
						}else if(kp->detail == 'Q'){
							camera_center[0] += 0.1f;
						}else if(kp->detail == 'j'){
							camera_center[1]-= 0.1f;
						}else if(kp->detail == 'Z'){
							camera_center[2]-= 0.1f;
						}else if(kp->detail == '['){
							camera_center[2] += 0.1f;
						}

						if( kp->detail == 'W'){
                            camera_eye[0]-= 0.1f;
						}else if(kp->detail == 'X'){
                            camera_eye[1]-= 0.1f;
						}else if(kp->detail == 'Y'){
                            camera_eye[0] += 0.1f;
						}else if(kp->detail == 'T'){
                            camera_eye[1] += 0.1f;
						}else if(kp->detail == 'S'){
                            camera_eye[2]-= 0.1f;
						}else if(kp->detail == 'U'){
                            camera_eye[2] += 0.1f;
						}

						if( kp->detail == 'r'){
							camera_up[0] += 0.1f;
						}else if(kp->detail == 'q'){
							camera_up[0]-= 0.1f;
						}else if(kp->detail == 'o'){
							camera_up[1] += 0.1f;
						}else if(kp->detail == 't'){
							camera_up[1]-= 0.1f;
						}else if(kp->detail == 'u'){
							camera_up[2]-= 0.1f;
						}else if(kp->detail == 'p'){
							camera_up[2] += 0.1f;
						}

						std::cout << kp->detail << std::endl;
						for(int i =0; i < descriptor_set.size(); i++)
						{
                            descriptor_set[1]->getUniformBuffer()->rotateCamera(glm::vec3(1,0,0));
							descriptor_set[1]->getUniformBuffer()->setCameraViewCenter(glm::vec3(camera_center[0], camera_center[1], camera_center[2]));
							descriptor_set[1]->getUniformBuffer()->setCameraViewEye(glm::vec3(camera_eye[0], camera_eye[1], camera_eye[2]));
							descriptor_set[1]->getUniformBuffer()->setCameraViewUp(glm::vec3(camera_up[0], camera_up[1], camera_up[2]));
						}

						return WindowEvent::ButtonDown;
					} else if((e->response_type & ~0x80) == XCB_KEY_RELEASE) {
						return WindowEvent::ButtonUp;
					} else {
						return WindowEvent::Unknow;
					}
				}

				return WindowEvent::None;
			}
		};
	}
}

#endif //OBSIDIAN2D_RENDERER_XCBWINDOW_
