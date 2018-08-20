#ifndef OBSIDIAN2D_RENDERER_XCBWINDOW_
#define OBSIDIAN2D_RENDERER_XCBWINDOW_

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <bitset>
#include <xcb/xcb.h>

#include "Window/Window.h"
#include <unistd.h>

namespace Engine
{
	namespace Window
	{
		struct {
			bool left = false;
			bool right = false;
			bool middle = false;
		} mouseButtons;

		class XcbWindow: public Window
		{

		public:

			XcbWindow(u_int32_t width, u_int32_t height)
			{
				this->width  = width;
				this->height = height;

			    this->createApplication();
                this->setConnection();
                this->createWindow();
                this->createSurface();

                this->init();
			}

			~XcbWindow()
            {
            	// @TODO Destroy XCB window after vkDestroyInstance()
				//xcb_destroy_window(connection, window);
				//xcb_disconnect(connection);
			}

		private:

			xcb_screen_t* 			screen{};
			xcb_window_t 			window{};
			xcb_connection_t*		connection{};
			glm::vec2 				mousePos;

			void setConnection()
			{
				const xcb_setup_t *setup;
				xcb_screen_iterator_t iter;
				int scr;

				connection = xcb_connect(nullptr, &scr);
				if (connection == nullptr || xcb_connection_has_error(connection)) {
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
								XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_LEAVE_WINDOW |
								XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION;

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

				// Force the x/y coordinates to 100, 100 results are identical in consecutive runs
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

				assert(vkCreateXcbSurfaceKHR(instance, &createInfo, nullptr, &surface) == VK_SUCCESS);
			}

		public:

			WindowEvent poolEvent() override
			{
			    xcb_generic_event_t* e = nullptr;
				while ((e = xcb_poll_for_event(connection)))
				{
					std::vector<Engine::Descriptors::DescriptorSet*> descSets;
					for(auto& program_obj : programs) descSets.push_back(program_obj->descriptor_layout);

					switch (e->response_type & 0x7f)
					{
						case XCB_CLIENT_MESSAGE: {
							return WindowEvent::Close;
						}

						case XCB_MOTION_NOTIFY:
						{
							auto *motion = (xcb_motion_notify_event_t *) e;
							for (auto &descSet : descSets) {
								if (mouseButtons.left) {
									descSet->getUniformBuffer()->rotateWorld(
											glm::vec3( (mousePos.y - (float) motion->event_y) * 0.1f,
													   -(mousePos.x - (float) motion->event_x) * 0.1f, 0.0f)
									);
								}
								if (mouseButtons.middle) descSet->getUniformBuffer()->zoomCamera(-0.1);
								if (mouseButtons.right) descSet->getUniformBuffer()->zoomCamera(0.1);
							}

							mousePos = glm::vec2((float) motion->event_x, (float) motion->event_y);

							return WindowEvent::None;
						}

						case XCB_BUTTON_PRESS: {
							auto *press = (xcb_button_press_event_t *) e;
							if (press->detail == XCB_BUTTON_INDEX_1) mouseButtons.left = true;
							if (press->detail == XCB_BUTTON_INDEX_2) mouseButtons.middle = true;
							if (press->detail == XCB_BUTTON_INDEX_3) mouseButtons.right = true;

							return WindowEvent::None;
						}

						case XCB_BUTTON_RELEASE: {
							auto *press = (xcb_button_press_event_t *) e;
							if (press->detail == XCB_BUTTON_INDEX_1) mouseButtons.left = false;
							if (press->detail == XCB_BUTTON_INDEX_2) mouseButtons.middle = false;
							if (press->detail == XCB_BUTTON_INDEX_3) mouseButtons.right = false;

							return WindowEvent::None;
						}

						case XCB_KEY_PRESS: {

							auto *keyEvent = (const xcb_key_press_event_t *) e;
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
				free(e);

				return WindowEvent::None;
			}
		};
	}
}

#endif //OBSIDIAN2D_RENDERER_XCBWINDOW_
