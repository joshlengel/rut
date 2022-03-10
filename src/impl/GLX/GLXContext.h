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
        struct GLXData
        {
            ::Display *display;
            ::Window window;
            ::GLXContext context;
        }

        class GLXContext : public Context
        {
        public:
            GLXContext(::Display *display, int screen, ::Window window);
            virtual ~GLXContext();

            virtual uint32_t GetVersionMajor() const override;
            virtual uint32_t GetVersionMinor() const override;

            virtual void Begin() override;
            virtual void End() override;

            virtual uint64_t GetHandle() const override;

        private:
            GLXData m_data;

            uint32_t m_version_major, m_version_minor;
        };
    }
}

#endif