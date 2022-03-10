#include"X11Window.h"

#ifdef RUT_HAS_X11

#include"RUT/Event.h"
#include"RUT/Api.h"

#ifdef RUT_HAS_GLX
#include"impl/GLX/GLXContext.h"

#include<GL/glx.h>
#endif
#ifdef RUT_HAS_EGL
#include"impl/EGL/EGLContext.h"

#include<EGL/egl.h>
#endif
#ifdef RUT_HAS_VULKAN
#include"impl/Vulkan/VulkanX11Context.h"

#include<vulkan/vulkan.h>
#endif

#include<iostream>
#include<cstdlib>
#include<unordered_map>
#include<cstring>

#include<X11/Xutil.h>
#include<X11/keysym.h>

static const std::unordered_map<KeySym, rut::KeyCode> X11_KEY_CODES =
{
    { XK_a,         rut::KC_A           },
    { XK_b,         rut::KC_B           },
    { XK_c,         rut::KC_C           },
    { XK_d,         rut::KC_D           },
    { XK_e,         rut::KC_E           },
    { XK_f,         rut::KC_F           },
    { XK_g,         rut::KC_G           },
    { XK_h,         rut::KC_H           },
    { XK_i,         rut::KC_I           },
    { XK_j,         rut::KC_J           },
    { XK_k,         rut::KC_K           },
    { XK_l,         rut::KC_L           },
    { XK_m,         rut::KC_M           },
    { XK_n,         rut::KC_N           },
    { XK_o,         rut::KC_O           },
    { XK_p,         rut::KC_P           },
    { XK_q,         rut::KC_Q           },
    { XK_r,         rut::KC_R           },
    { XK_s,         rut::KC_S           },
    { XK_t,         rut::KC_T           },
    { XK_u,         rut::KC_U           },
    { XK_v,         rut::KC_V           },
    { XK_w,         rut::KC_W           },
    { XK_x,         rut::KC_X           },
    { XK_y,         rut::KC_Y           },
    { XK_z,         rut::KC_Z           },
    { XK_0,         rut::KC_0           },
    { XK_1,         rut::KC_1           },
    { XK_2,         rut::KC_2           },
    { XK_3,         rut::KC_3           },
    { XK_4,         rut::KC_4           },
    { XK_5,         rut::KC_5           },
    { XK_6,         rut::KC_6           },
    { XK_7,         rut::KC_7           },
    { XK_8,         rut::KC_8           },
    { XK_9,         rut::KC_9           },
    { XK_space,     rut::KC_SPACE       },
    { XK_Return,    rut::KC_ENTER       },
    { XK_BackSpace, rut::KC_BACKSPACE   },
    { XK_Tab,       rut::KC_TAB         },
    { XK_Caps_Lock, rut::KC_CAPS_LOCK   },
    { XK_Shift_L,   rut::KC_LEFT_SHIFT  },
    { XK_Shift_R,   rut::KC_RIGHT_SHIFT },
    { XK_Control_L, rut::KC_LEFT_CTRL   },
    { XK_Control_R, rut::KC_RIGHT_CTRL  },
    { XK_Alt_L,     rut::KC_LEFT_ALT    },
    { XK_Alt_R,     rut::KC_RIGHT_ALT   },
    { XK_Home,      rut::KC_HOME        },
    { XK_Left,      rut::KC_LEFT        },
    { XK_Right,     rut::KC_RIGHT       },
    { XK_Down,      rut::KC_DOWN        },
    { XK_Up,        rut::KC_UP          },
    { XK_Escape,    rut::KC_ESCAPE      },
    { XK_Insert,    rut::KC_INSERT      }
};

static const std::unordered_map<unsigned int, rut::MouseButtonCode> X11_MOUSE_BUTTON_CODES =
{
    { Button1, rut::MBC_LEFT },
    { Button2, rut::MBC_RIGHT }
};

namespace rut
{
    namespace impl
    {
        X11Window::X11Window(const WindowProperties &props):
            m_props(props),
            m_handler([](Event &e){})
        {
            // Get screen
            m_display = XOpenDisplay(nullptr);
            m_screen = DefaultScreen(m_display);
            unsigned long white_pixel = WhitePixel(m_display, m_screen);
            unsigned long black_pixel = BlackPixel(m_display, m_screen);

            // Create window
            m_window = XCreateSimpleWindow(m_display, DefaultRootWindow(m_display),
                0, 0, props.width, props.height,
                5, white_pixel, black_pixel);
            
            // Change title
            XSetStandardProperties(m_display, m_window, props.title.c_str(), props.title.c_str(), None, nullptr, 0, nullptr);

            XSelectInput(m_display, m_window, ExposureMask | ResizeRedirectMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask);

            // Register close event
            Atom wm_delete_window = XInternAtom(m_display, "WM_DELETE_WINDOW", false);
            XSetWMProtocols(m_display, m_window, &wm_delete_window, 1);

            // Create context
            m_context_api = Api::GetContextApi();
#ifdef RUT_HAS_GLX
            if (m_context_api == CONTEXT_API_GLX)
                m_context = new GLXContext(m_display, m_screen, m_window);
#endif
#ifdef RUT_HAS_EGL
            if (m_context_api == CONTEXT_API_EGL)
                m_context = new EGLContext(reinterpret_cast<void*>(m_display), reinterpret_cast<void*>(m_window));
#endif
#ifdef RUT_HAS_VULKAN
            if (m_context_api == CONTEXT_API_KHR_SURFACE)
                m_context = new VulkanX11Context(m_display, m_window, m_props.width, m_props.height);
#endif

            // Clear state
            m_should_close = false;
            std::memset(m_keys, 0, sizeof(m_keys));
            std::memset(m_mouse_buttons, 0, sizeof(m_mouse_buttons));
        }

        X11Window::~X11Window()
        {
            delete m_context;

            XDestroyWindow(m_display, m_window);
            XCloseDisplay(m_display);
        }

        const WindowProperties &X11Window::GetProps() const { return m_props; }

        void X11Window::Show()
        {
            XMapRaised(m_display, m_window);
        }

        void X11Window::Update()
        {
            // Poll events
            while (XPending(m_display))
            {
                XEvent event;
                XNextEvent(m_display, &event);
                OnEvent(event);
            }

            m_old_cursor_pos = m_cursor_pos;

            // Swap buffers
#ifdef RUT_HAS_GLX
            if (m_context_api == CONTEXT_API_GLX)
                glXSwapBuffers(m_display, m_window);
#endif
#ifdef RUT_HAS_EGL
            if (m_context_api == CONTEXT_API_EGL)
            {
                EGLData *data = reinterpret_cast<EGLData*>(m_context->GetHandle());
                eglSwapBuffers(data->display, data->surface);
            }
#endif
#ifdef RUT_HAS_VULKAN
            if (m_context_api == CONTEXT_API_KHR_SURFACE)
            {
                VulkanData *data = reinterpret_cast<VulkanData*>(m_context->GetHandle());
                SwapVulkanBuffers(data);
                
                if (!data->swapchain_renderable)
                    SetupVulkanSwapchain(m_props.width, m_props.height, data);
            }
#endif
        }

        void X11Window::OnEvent(XEvent &event)
        {
            Event *dispatch_event = nullptr;
            switch (event.type)
            {
                case Expose:
                    //! TODO: Redraw
                    break;
                
                case ResizeRequest:
                {
                    m_props.width = event.xresizerequest.width;
                    m_props.height = event.xresizerequest.height;
#ifdef RUT_HAS_VULKAN
                    if (m_context_api == CONTEXT_API_KHR_SURFACE)
                    {
                        VulkanData *data = reinterpret_cast<VulkanData*>(m_context->GetHandle());
                        data->swapchain_renderable = false;
                        std::cout << "width: " << m_props.width << ", height: " << m_props.height << std::endl;
                    }
#endif
                    break;
                }
                
                case ClientMessage:
                {
                    // Check if window closed
                    Atom wm_delete_window = XInternAtom(m_display, "WM_DELETE_WINDOW", false);
                    if (static_cast<Atom>(event.xclient.data.l[0]) == wm_delete_window)
                    {
                        m_should_close = true;
                        dispatch_event = new WindowClosedEvent(shared_from_this());
                    }
                    
                    break;
                }

                case KeyPress:
                case KeyRelease:
                {
                    char text[255];
                    KeySym sym, lower, upper;
                    if (!XLookupString(&event.xkey, text, 255, &sym, nullptr))
                        break;
                    
                    XConvertCase(sym, &lower, &upper);

                    auto itr = X11_KEY_CODES.find(lower);
                    if (itr == X11_KEY_CODES.end())
                        break;
                    
                    bool is_pressed = event.type == KeyPress;
                    m_keys[itr->second] = is_pressed;
                    dispatch_event = is_pressed?
                        dynamic_cast<Event*>(new KeyPressedEvent(itr->second)) :
                        dynamic_cast<Event*>(new KeyReleasedEvent(itr->second));
                    break;
                }

                case ButtonPress:
                case ButtonRelease:
                {
                    auto itr = X11_MOUSE_BUTTON_CODES.find(event.xbutton.button);
                    if (itr == X11_MOUSE_BUTTON_CODES.end())
                        break;
                    
                    bool is_pressed = event.type == ButtonPress;
                    m_mouse_buttons[itr->second] = is_pressed;
                    dispatch_event = is_pressed?
                        dynamic_cast<Event*>(new MouseButtonPressedEvent(itr->second)) :
                        dynamic_cast<Event*>(new MouseButtonReleasedEvent(itr->second));
                    break;
                }

                case MotionNotify:
                {
                    float x = event.xmotion.x;
                    float y = event.xmotion.y;
                    dispatch_event = new CursorMovedEvent(x, y, x - m_old_cursor_pos.x, y - m_old_cursor_pos.y);
                    m_cursor_pos = { x, y };
                    break;
                }

                default:
                    break;
            }

            if (dispatch_event)
            {
                m_handler(*dispatch_event);
                delete dispatch_event;
            }
        }

        bool X11Window::ShouldClose() const { return m_should_close; }
        void X11Window::SetEventHandler(const EventHandler &handler) { m_handler = handler; }

        bool X11Window::KeyDown(KeyCode key_code) const { return m_keys[key_code]; }
        bool X11Window::MouseButtonDown(MouseButtonCode mouse_button_code) const { return m_mouse_buttons[mouse_button_code]; }
        glm::vec2 X11Window::GetCursorPosition() const { return m_cursor_pos; }

        Context *X11Window::GetContext() const { return m_context; }
        uint64_t X11Window::GetHandle() const { return reinterpret_cast<uint64_t>(m_display); }
    }
}

#endif