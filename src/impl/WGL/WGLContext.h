#pragma once

#include"RUT/Config.h"

#ifdef RUT_HAS_WGL

#include"RUT/Context.h"

#include<Windows.h>

namespace rut
{
    namespace impl
    {
        struct WGLData
        {
            HDC device;
            HGLRC context;
            bool context_renderable = false;
        };

        class WGLContext : public Context
        {
        public:
            WGLContext(HWND window);
            virtual ~WGLContext();

            virtual uint32_t GetVersionMajor() const override;
            virtual uint32_t GetVersionMinor() const override;

            virtual void Begin() override;
            virtual void End() override;

            virtual uint64_t GetHandle() const override;
        
        private:
            WGLData m_data;
            uint32_t m_version_major, m_version_minor;
        };
    }
}

#endif