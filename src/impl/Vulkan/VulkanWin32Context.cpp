#include"VulkanWin32Context.h"

#if defined(RUT_HAS_VULKAN) && defined(RUT_HAS_WIN32)

#include<vector>
#include<stdexcept>

namespace rut
{
    namespace impl
    {
        VulkanWin32Context::VulkanWin32Context(HINSTANCE instance, HWND window, uint32_t width, uint32_t height)
        {
            std::vector<const char*> required_extensions =
            {
                VK_KHR_SURFACE_EXTENSION_NAME,
                VK_KHR_WIN32_SURFACE_EXTENSION_NAME
            };
            CreateVulkanInstance(required_extensions.size(), required_extensions.data(), &m_data, m_version_major, m_version_minor);

            VkWin32SurfaceCreateInfoKHR surface_create_info{};
            surface_create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
            surface_create_info.hinstance = instance;
            surface_create_info.hwnd = window;
            surface_create_info.flags = 0;

            if (vkCreateWin32SurfaceKHR(m_data.instance, &surface_create_info, nullptr, &m_data.surface) != VK_SUCCESS)
                throw std::runtime_error("Error creating Vulkan context: vkCreateWin32SurfaceKHR failed");

            std::vector<const char*> required_device_extensions =
            {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME
            };
            SetupVulkanDevice(required_device_extensions.size(), required_device_extensions.data(), &m_data);
            SetupVulkanSyncObjects(&m_data);
            SetupVulkanSwapchain(width, height, &m_data);
        }

        VulkanWin32Context::~VulkanWin32Context()
        {
            DestroyVulkanInstance(&m_data);
        }

        uint32_t VulkanWin32Context::GetVersionMajor() const { return m_version_major; }
        uint32_t VulkanWin32Context::GetVersionMinor() const { return m_version_minor; }

        void VulkanWin32Context::Begin()
        {
            BeginVulkanContext(&m_data);
        }

        void VulkanWin32Context::End()
        {
            EndVulkanContext(&m_data);
        }

        uint64_t VulkanWin32Context::GetHandle() const { return reinterpret_cast<uint64_t>(&m_data); }
    }
}

#endif