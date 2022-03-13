#pragma once

#include"RUT/Config.h"

#ifdef RUT_HAS_VULKAN

#include<vector>
#include<optional>

#include<vulkan/vulkan.h>

static const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

namespace rut
{
    namespace impl
    {
        struct VulkanQueueFamilyIndices
        {
            std::optional<uint32_t> graphics_family;
            std::optional<uint32_t> present_family;

            bool IsComplete() const
            {
                return graphics_family.has_value() && present_family.has_value();
            }
        };

        struct VulkanData
        {
            VkInstance instance;
            VkDebugUtilsMessengerEXT debug_messenger;
            VkSurfaceKHR surface;
            VkPhysicalDevice physical_device;
            VkDevice device;
            VkQueue graphics_queue, present_queue;

            bool have_swapchain = false;
            VkSwapchainKHR swapchain;
            VkFormat swapchain_format;
            VkExtent2D swapchain_extent;
            std::vector<VkImage> swapchain_images;
            std::vector<VkImageView> swapchain_image_views;
            std::vector<VkFramebuffer> swapchain_framebuffers;
            uint32_t current_image_index;
            VkRenderPass render_pass;

            VkCommandPool cmd_pool;
            std::vector<VkCommandBuffer> cmd_buffers;

            std::vector<VkSemaphore> image_available_sems, render_finished_sems;
            std::vector<VkFence> in_flight_fences;
            uint32_t current_frame = 0;
            bool swapchain_renderable = false;
        };

        void GetVulkanQueueFamilies(VkPhysicalDevice physical_device, VkSurfaceKHR surface, VulkanQueueFamilyIndices &indices);
        uint32_t GetVulkanMemoryType(VkPhysicalDevice physical_device, uint32_t filter, VkMemoryPropertyFlags flags);
        void CreateVulkanInstance(uint32_t num_extensions, const char *const *extensions, VulkanData *dst, uint32_t &version_major, uint32_t &version_minor);
        void SetupVulkanDevice(uint32_t num_extensions, const char *const *extensions, VulkanData *data);
        void SetupVulkanSwapchain(uint32_t width, uint32_t height, VulkanData *data);
        void SetupVulkanSyncObjects(VulkanData *data);
        void BeginVulkanContext(VulkanData *data);
        void EndVulkanContext(VulkanData *data);
        void DestroyVulkanInstance(VulkanData *data);
    }
}

#endif