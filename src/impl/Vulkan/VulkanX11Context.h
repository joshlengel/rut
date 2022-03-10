#pragma once

#include"RUT/Config.h"

#if defined(RUT_HAS_VULKAN) && defined(RUT_HAS_X11)

#define VK_USE_PLATFORM_XLIB_KHR
#define VK_PROTOTYPES
#include"impl/Vulkan/VulkanUtils.h"
#include"RUT/Context.h"

#include<memory>

#include<X11/Xlib.h>

namespace rut
{
    namespace impl
    {
        class VulkanX11Context : public Context, public std::enable_shared_from_this<VulkanX11Context>
        {
        public:
            VulkanX11Context(::Display *display, ::Window window, uint32_t width, uint32_t height);
            virtual ~VulkanX11Context();

            virtual uint32_t GetVersionMajor() const override;
            virtual uint32_t GetVersionMinor() const override;

            virtual void Begin() override;
            virtual void End() override;

            virtual uint64_t GetHandle() const override;
        
        private:
            VulkanData m_data;
            uint32_t m_version_major, m_version_minor;
        };
    }
}

#endif