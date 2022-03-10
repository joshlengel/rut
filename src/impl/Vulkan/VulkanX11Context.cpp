#include"VulkanX11Context.h"

#if defined(RUT_HAS_VULKAN) && defined(RUT_HAS_X11)

#include<vector>
#include<stdexcept>

namespace rut
{
    namespace impl
    {
        VulkanX11Context::VulkanX11Context(::Display *display, ::Window window, uint32_t width, uint32_t height)
        {
            std::vector<const char*> required_extensions =
            {
                VK_KHR_SURFACE_EXTENSION_NAME,
                VK_KHR_XLIB_SURFACE_EXTENSION_NAME
            };
            CreateVulkanInstance(required_extensions.size(), required_extensions.data(), &m_data, m_version_major, m_version_minor);

            VkXlibSurfaceCreateInfoKHR surface_create_info{};
            surface_create_info.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
            surface_create_info.dpy = display;
            surface_create_info.window = window;
            surface_create_info.flags = 0;

            if (vkCreateXlibSurfaceKHR(m_data.instance, &surface_create_info, nullptr, &m_data.surface) != VK_SUCCESS)
                throw std::runtime_error("Error creating Vulkan context: vkCreateXlibSurfaceKHR failed");
            
            std::vector<const char*> required_device_extensions =
            {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME
            };
            SetupVulkanDevice(required_device_extensions.size(), required_device_extensions.data(), &m_data);
            SetupVulkanSyncObjects(&m_data);
            SetupVulkanSwapchain(width, height, &m_data);
        }

        VulkanX11Context::~VulkanX11Context()
        {
            DestroyVulkanInstance(&m_data);
        }

        uint32_t VulkanX11Context::GetVersionMajor() const { return m_version_major; }
        uint32_t VulkanX11Context::GetVersionMinor() const { return m_version_minor; }

        void VulkanX11Context::Begin()
        {
            BeginVulkanContext(&m_data);
        }

        void VulkanX11Context::End()
        {
            EndVulkanContext(&m_data);
        }
        
        uint64_t VulkanX11Context::GetHandle() const { return reinterpret_cast<uint64_t>(&m_data); }
    }
}

#endif