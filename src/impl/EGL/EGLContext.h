#pragma once

#include"RUT/Config.h"

#ifdef RUT_HAS_EGL

#include"RUT/Context.h"

#include<EGL/egl.h>

namespace rut
{
    namespace impl
    {
        struct EGLData
        {
            ::EGLDisplay display;
            ::EGLContext context;
            ::EGLSurface surface;
        };

        class EGLContext : public Context
        {
        public:
            EGLContext(void *native_display, void *native_window);
            virtual ~EGLContext();

            uint32_t GetVersionMajor() const override;
            uint32_t GetVersionMinor() const override;

            uint64_t GetHandle() const override;
        
        private:
            EGLData *m_egl_data;

            uint32_t m_version_major, m_version_minor;
        };
    }
}

#endif