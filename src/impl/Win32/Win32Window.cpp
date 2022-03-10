#include"Win32Window.h"

#ifdef RUT_HAS_WIN32

#include"RUT/Event.h"

#ifdef RUT_HAS_WGL
#include"impl/WGL/WGLContext.h"
#include"impl/OpenGL/OpenGLUtils.h"
#endif
#ifdef RUT_HAS_EGL
#include"impl/EGL/EGLContext.h"
#include"impl/OpenGL/OpenGLUtils.h"
#endif
#ifdef RUT_HAS_VULKAN
#include"impl/Vulkan/VulkanX11Context.h"
#include"impl/Vulkan/VulkanUtils.h"
#endif

#include<stdexcept>
#include<iostream>

static const char *RUT_WIN32_CLASS_NAME = "RUT Window";

namespace rut
{
    namespace impl
    {
        Win32Window::Win32Window(const WindowProperties &props):
            m_props(props),
            m_context_api(Api::GetContextApi()),
            m_should_close(false),
            m_handler([](Event&){})
        {
            m_instance = GetModuleHandle(nullptr);

            WNDCLASSEX win_class{};
            win_class.cbSize = sizeof(WNDCLASSEX);
            win_class.lpfnWndProc = &Win32Window::WindowProc;
            win_class.lpszClassName = RUT_WIN32_CLASS_NAME;
            win_class.hInstance = m_instance;

            RegisterClassEx(&win_class);

            m_window = CreateWindowEx(WS_EX_CLIENTEDGE, win_class.lpszClassName, m_props.title.c_str(), WS_OVERLAPPEDWINDOW,
                CW_USEDEFAULT, CW_USEDEFAULT, m_props.width, m_props.height,
                nullptr, nullptr, m_instance, nullptr);
            if (!m_window)
                throw std::runtime_error("Error creating Win32 window: handle is null");
            
            SetWindowLongPtr(m_window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
            SetWindowPos(m_window, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

            // Create context
#ifdef RUT_HAS_WGL
            if (m_context_api == CONTEXT_API_WGL)
                m_context = new WGLContext(m_window);
#endif
#ifdef RUT_HAS_EGL
            if (m_context_api == CONTEXT_API_EGL)
                m_context = new EGLContext(m_instance, m_window);
#endif
        }

        Win32Window::~Win32Window()
        {
            delete m_context;

            DestroyWindow(m_window);
            UnregisterClass(RUT_WIN32_CLASS_NAME, m_instance);
        }

        const WindowProperties &Win32Window::GetProps() const { return m_props; }

        LRESULT Win32Window::WindowProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
        {
            Win32Window *win = reinterpret_cast<Win32Window*>(GetWindowLongPtr(window, GWLP_USERDATA));
            switch (message)
            {
                case WM_DESTROY:
                {
                    WindowClosedEvent event(win->shared_from_this());
                    win->m_handler(event);
                    win->m_should_close = true;
                    return 0;
                }
                
                case WM_SIZE:
                    win->m_props.width = LOWORD(l_param);
                    win->m_props.height = HIWORD(l_param);

#ifdef RUT_HAS_WGL
                    if (win->m_context_api == CONTEXT_API_WGL)
                    {
                        WGLData *data = reinterpret_cast<WGLData*>(win->m_context->GetHandle());
                        data->context_renderable = false;
                    }
#endif
#ifdef RUT_HAS_EGL
                    if (win->m_context_api == CONTEXT_API_EGL)
                    {
                        EGLData *data = reinterpret_cast<EGLData*>(win->m_context->GetHandle());
                        data->context_renderable = false;
                    }
#endif
#ifdef RUT_HAS_VULKAN
                    if (win->m_context_api == CONTEXT_API_KHR_SURFACE)
                    {
                        VulkanData *data = reinterpret_cast<VulkanData*>(win->m_context->GetHandle());
                        data->swapchain_renderable = false;
                    }
#endif
                    return 0;
                
                default:
                    return DefWindowProc(window, message, w_param, l_param);
            }
        }

        void Win32Window::Show()
        {
            ShowWindow(m_window, SW_SHOW);
        }

        void Win32Window::PollEvents()
        {
            // Check events
            MSG msg{};
            while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

#ifdef RUT_HAS_WGL
            if (m_context_api == CONTEXT_API_WGL)
            {
                WGLData *data = reinterpret_cast<WGLData*>(m_context->GetHandle());
                if (!data->context_renderable)
                {
                    glViewport(0, 0, m_props.width, m_props.height);
                    data->context_renderable = true;
                }
            }
#endif
#ifdef RUT_HAS_EGL
            if (m_context_api == CONTEXT_API_EGL)
            {
                EGLData *data = reinterpret_cast<EGLData*>(m_context->GetHandle());
                if (!data->context_renderable)
                {
                    glViewport(0, 0, m_props.width, m_props.height);
                    data->context_renderable = true;
                }
            }
#endif
#ifdef RUT_HAS_VULKAN
            if (m_context_api == CONTEXT_API_KHR_SURFACE)
            {
                VulkanData *data = reinterpret_cast<VulkanData*>(m_context->GetHandle());
                if (!data->swapchain_renderable)
                    SetupVulkanSwapchain(m_props.width, m_props.height, data);
            }
#endif
        }

        bool Win32Window::ShouldClose() const { return m_should_close; }
        void Win32Window::SetEventHandler(const EventHandler &handler) { m_handler = handler; }

        bool Win32Window::KeyDown(KeyCode key_code) const { return false; }
        bool Win32Window::MouseButtonDown(MouseButtonCode mouse_button_code) const { return false; }
        glm::vec2 Win32Window::GetCursorPosition() const { return { 0, 0 }; }

        Context *Win32Window::GetContext() const { return m_context; }
        uint64_t Win32Window::GetHandle() const { return reinterpret_cast<uint64_t>(m_window); }
    }
}

#endif