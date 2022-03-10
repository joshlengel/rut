#include"GLXContext.h"

#ifdef RUT_HAS_GLX

#include"impl/OpenGL/OpenGLUtils.h"

#include<stdexcept>

#include<X11/Xutil.h>

namespace rut
{
    namespace impl
    {
        GLXContext::GLXContext(::Display *display, int screen, ::Window window)
        {
            m_data.display = display;
            m_data.window = window;

            int attrib_list[] =
            {
                GLX_RGBA,
                GLX_DEPTH_SIZE, 24,
                GLX_DOUBLEBUFFER,
                None
            };
            XVisualInfo *visual = glXChooseVisual(m_data.display, screen, attrib_list);
            m_data.context = glXCreateContext(display, visual, 0, true);
            glXMakeCurrent(m_data.display, m_data.window, m_data.context);

            GLint major, minor;
            glGetIntegerv(GL_MAJOR_VERSION, &major);
            glGetIntegerv(GL_MINOR_VERSION, &minor);
            m_version_major = major;
            m_version_minor = minor;

            LoadOpenGLFunctions(reinterpret_cast<Proc(*)(const char*)>(&glXGetProcAddress));

            if (m_version_major < 3 || (m_version_major == 3 && m_version_minor < 3))
                throw std::runtime_error("Device only supports OpenGL version " + std::to_string(m_version_major) + "." + std::to_string(m_version_major) + " - a minimum of 3.3 is required.");
        }

        GLXContext::~GLXContext()
        {
            glXDestroyContext(m_data.display, m_data.context);
        }

        uint32_t GLXContext::GetVersionMajor() const { return m_version_major; }
        uint32_t GLXContext::GetVersionMinor() const { return m_version_minor; }

        void GLXContext::Begin()
        {}

        void GLXContext::End()
        {
            glXSwapBuffers(m_data.display, m_data.window);
        }

        uint64_t GLXContext::GetHandle() const { return reinterpret_cast<uint64_t>(&m_data); }
    }
}
#endif