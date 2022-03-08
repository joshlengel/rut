#pragma once

#include"RUT/Config.h"

#ifdef RUT_HAS_GLX

#include"RUT/Context.h"

#include<X11/Xlib.h>
#include<GL/glx.h>

namespace rut
{
    namespace impl
    {
        class GLXContext : public Context
        {
        public:
            GLXContext(::Display *display, int screen, ::Window window);
            virtual ~GLXContext();

            virtual uint32_t GetVersionMajor() const override;
            virtual uint32_t GetVersionMinor() const override;

            virtual uint64_t GetHandle() const override;

        private:
            ::Display *m_display;
            ::GLXContext m_context;

            uint32_t m_version_major, m_version_minor;
        };
    }
}

#endif