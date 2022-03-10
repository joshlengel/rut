#include"EGLContext.h"

#ifdef RUT_HAS_EGL

#include"impl/OpenGL/OpenGLUtils.h"

#include<stdexcept>

namespace rut
{
    namespace impl
    {
        EGLContext::EGLContext(void *native_display, void *native_window)
        {
            m_data.display = eglGetDisplay(reinterpret_cast<EGLNativeDisplayType>(native_display));
            if (!m_data.display)
                throw std::runtime_error("Error getting EGL display: " + std::to_string(eglGetError()));

            if (!eglInitialize(m_data.display, nullptr, nullptr))
                throw std::runtime_error("Error initializing EGL: " + std::to_string(eglGetError()));
            
            EGLint attr[] =
            {
                EGL_BUFFER_SIZE, 0,
                EGL_RED_SIZE,    8,
                EGL_GREEN_SIZE,  8,
                EGL_BLUE_SIZE,   8,
                EGL_ALPHA_SIZE,  8,
                EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER,
                EGL_DEPTH_SIZE, 24,
                EGL_LEVEL, 0,
                EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
                EGL_SAMPLE_BUFFERS, 0,
                EGL_SAMPLES,        0,
                EGL_STENCIL_SIZE,   8,
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_TRANSPARENT_TYPE, EGL_NONE,
                EGL_TRANSPARENT_RED_VALUE, EGL_DONT_CARE,
                EGL_TRANSPARENT_GREEN_VALUE, EGL_DONT_CARE,
                EGL_TRANSPARENT_BLUE_VALUE, EGL_DONT_CARE,
                EGL_CONFIG_CAVEAT, EGL_DONT_CARE,
                EGL_CONFIG_ID, EGL_DONT_CARE,
                EGL_MAX_SWAP_INTERVAL, EGL_DONT_CARE,
                EGL_MIN_SWAP_INTERVAL, EGL_DONT_CARE,
                EGL_NATIVE_RENDERABLE, EGL_DONT_CARE,
                EGL_NATIVE_VISUAL_TYPE, EGL_DONT_CARE,
                EGL_NONE
            };
            EGLConfig config;
            EGLint num_config;
            if (!eglChooseConfig(m_data.display, attr, &config, 1, &num_config))
                throw std::runtime_error("Error choosing EGL config: " + std::to_string(eglGetError()));
            
            if (num_config == 0)
                throw std::runtime_error("No EGL configs available");
            
            m_data.surface = eglCreateWindowSurface(m_data.display, config, reinterpret_cast<EGLNativeWindowType>(native_window), nullptr);
            if (!m_data.surface)
                throw std::runtime_error("Error creating EGL surface: " + std::to_string(eglGetError()));

            eglBindAPI(EGL_OPENGL_API);

            EGLint ctxattr[] =
            {
                EGL_CONTEXT_CLIENT_VERSION, 3,
                EGL_NONE
            };
            m_data.context = eglCreateContext(m_data.display, config, static_cast<::EGLContext>(0), ctxattr);
            if (!m_data.context)
                throw std::runtime_error("Error creating EGL context: " + std::to_string(eglGetError()));
            
            eglMakeCurrent(m_data.display, m_data.surface, m_data.surface, m_data.context);

            GLint major, minor;
            glGetIntegerv(GL_MAJOR_VERSION, &major);
            glGetIntegerv(GL_MINOR_VERSION, &minor);
            m_version_major = major;
            m_version_minor = minor;

            LoadOpenGLFunctions(reinterpret_cast<Proc(*)(const char*)>(&eglGetProcAddress));

            if (m_version_major < 3 || (m_version_major == 3 && m_version_minor < 3))
                throw std::runtime_error("Device only supports OpenGL version " + std::to_string(m_version_major) + "." + std::to_string(m_version_major) + " - a minimum of 3.3 is required.");
        }

        EGLContext::~EGLContext()
        {
            eglDestroyContext(m_data.display, m_data.context);
            eglDestroySurface(m_data.display, m_data.surface);
            eglTerminate(m_data.display);
        }

        uint32_t EGLContext::GetVersionMajor() const { return m_version_major; }
        uint32_t EGLContext::GetVersionMinor() const { return m_version_minor; }

        void EGLContext::Begin()
        {}

        void EGLContext::End()
        {
            eglSwapBuffers(m_data.display, m_data.surface);
        }


        uint64_t EGLContext::GetHandle() const { return reinterpret_cast<uint64_t>(&m_data); }
    }
}

#endif