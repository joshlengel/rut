#pragma once

#include"RUT/Config.h"

#ifdef RUT_HAS_WGL

#include"RUT/Context.h"

#include<Windows.h>

namespace rut
{
    namespace impl
    {
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
            HDC m_device;
            HGLRC m_context;
            uint32_t m_version_major, m_version_minor;
        };
    }
}

#endif