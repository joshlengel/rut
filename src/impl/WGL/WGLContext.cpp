#include"WGLContext.h"

#ifdef RUT_HAS_WGL

#include"impl/OpenGL/OpenGLUtils.h"

#include<string>
#include<stdexcept>

namespace rut
{
    namespace impl
    {
        WGLContext::WGLContext(HWND window)
        {
            m_data.device = GetDC(window);

            PIXELFORMATDESCRIPTOR pfd =
            {
                sizeof(PIXELFORMATDESCRIPTOR),
                1,
                PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    // Flags
                PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
                32,                   // Colordepth of the framebuffer.
                0, 0, 0, 0, 0, 0,
                0,
                0,
                0,
                0, 0, 0, 0,
                24,                   // Number of bits for the depthbuffer
                8,                    // Number of bits for the stencilbuffer
                0,                    // Number of Aux buffers in the framebuffer.
                PFD_MAIN_PLANE,
                0,
                0, 0, 0
            };

            int pixel_format_index = ChoosePixelFormat(m_data.device, &pfd);
            SetPixelFormat(m_data.device, pixel_format_index, &pfd);

            m_data.context = wglCreateContext(m_data.device);
            if (!m_data.context)
                throw std::runtime_error("Error creating WGL context: " + std::to_string(GetLastError()));
            wglMakeCurrent(m_data.device, m_data.context);

            GLint major, minor;
            glGetIntegerv(GL_MAJOR_VERSION, &major);
            glGetIntegerv(GL_MINOR_VERSION, &minor);
            m_version_major = major;
            m_version_minor = minor;

            // Must wrap wglGetProcAddress due to calling convention
            LoadOpenGLFunctions([](const char *name) -> Proc { return reinterpret_cast<Proc>(wglGetProcAddress(name)); });

            if (m_version_major < 3 || (m_version_major == 3 && m_version_minor < 3))
                throw std::runtime_error("Device only supports OpenGL version " + std::to_string(m_version_major) + "." + std::to_string(m_version_major) + " - a minimum of 3.3 is required.");
        }

        WGLContext::~WGLContext()
        {
            wglDeleteContext(m_data.context);
        }

        uint32_t WGLContext::GetVersionMajor() const { return m_version_major; }
        uint32_t WGLContext::GetVersionMinor() const { return m_version_minor; }

        void WGLContext::Begin()
        {}

        void WGLContext::End()
        {
            wglSwapLayerBuffers(m_data.device, WGL_SWAP_MAIN_PLANE);
        }

        uint64_t WGLContext::GetHandle() const { return reinterpret_cast<uint64_t>(&m_data); }
    }
}

#endif