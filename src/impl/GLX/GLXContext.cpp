#include"GLXContext.h"

#ifdef RUT_HAS_GLX

#include"impl/OpenGL/OpenGLUtils.h"

#include<stdexcept>

#include<X11/Xutil.h>

namespace rut
{
    namespace impl
    {
        GLXContext::GLXContext(::Display *display, int screen, ::Window window):
            m_display(display),
            m_window(window)
        {
            int attrib_list[] =
            {
                GLX_RGBA,
                GLX_DEPTH_SIZE, 24,
                GLX_DOUBLEBUFFER,
                None
            };
            XVisualInfo *visual = glXChooseVisual(m_display, screen, attrib_list);
            m_context = glXCreateContext(display, visual, 0, true);
            glXMakeCurrent(m_display, m_window, m_context);

            LoadOpenGLFunctions(reinterpret_cast<LoadProc>(&glXGetProcAddress));

            GLint major, minor;
            glGetIntegerv(GL_MAJOR_VERSION, &major);
            glGetIntegerv(GL_MINOR_VERSION, &minor);
            m_version_major = major;
            m_version_minor = minor;

            if (m_version_major < 3 || (m_version_major == 3 && m_version_minor < 3))
                throw std::runtime_error("Device only supports OpenGL version " + std::to_string(m_version_major) + "." + std::to_string(m_version_major) + " - a minimum of 3.3 is required.");
        }

        GLXContext::~GLXContext()
        {
            glXDestroyContext(m_display, m_context);
        }

        uint32_t GLXContext::GetVersionMajor() const { return m_version_major; }
        uint32_t GLXContext::GetVersionMinor() const { return m_version_minor; }

        void GLXContext::Begin()
        {}

        void GLXContext::End()
        {
            glXSwapBuffers(m_display, m_window);
        }

        uint64_t GLXContext::GetHandle() const { return reinterpret_cast<uint64_t>(m_context); }
    }
}
#endif