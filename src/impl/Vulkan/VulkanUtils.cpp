#include"VulkanUtils.h"

#ifdef RUT_HAS_VULKAN

#include"RUT/Api.h"

#include<stdexcept>
#include<vector>
#include<cstring>
#include<unordered_map>
#include<optional>
#include<set>

#ifdef RUT_BUILD_DEBUG
#include<iostream>

static const std::vector<const char*> REQUIRED_EXTENSIONS =
{
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME
};
static const std::vector<const char*> REQUIRED_LAYERS =
{
    "VK_LAYER_KHRONOS_validation"
};

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    std::cerr << "Vulkan validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

static void SetVulkanDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &create_info)
{
    create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    create_info.pfnUserCallback = &DebugCallback;
    create_info.pUserData = nullptr; // Optional
}

#else
static const std::vector<const char*> REQUIRED_EXTENSIONS{};
static const std::vector<const char*> REQUIRED_LAYERS{};
#endif

namespace rut
{
    namespace impl
    {
        struct VulkanSwapchainDetails
        {
            VkSurfaceCapabilitiesKHR capabilities;
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> present_modes;

            bool IsAdequate() const
            {
                return !formats.empty() && !present_modes.empty();
            }

            VkSurfaceFormatKHR ChooseFormat() const
            {
                for (const VkSurfaceFormatKHR &format : formats)
                {
                    if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                        return format;
                }

                return formats[0];
            }

            VkPresentModeKHR ChoosePresentMode() const
            {
                for (const VkPresentModeKHR &present_mode : present_modes)
                {
                    if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
                        return present_mode;
                }

                return VK_PRESENT_MODE_FIFO_KHR;
            }

            VkExtent2D ChooseExtent(uint32_t width, uint32_t height) const
            {
                if (capabilities.currentExtent.width != UINT32_MAX)
                    return capabilities.currentExtent;
                
                VkExtent2D extent;
                extent.width = std::min(std::max(width, capabilities.minImageExtent.width), capabilities.maxImageExtent.width);
                extent.height = std::min(std::max(height, capabilities.minImageExtent.height), capabilities.maxImageExtent.height);
                return extent;
            }
        };

        void GetVulkanQueueFamilies(VkPhysicalDevice physical_device, VkSurfaceKHR surface, VulkanQueueFamilyIndices &indices)
        {
            uint32_t num_queue_families;
            vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &num_queue_families, nullptr);
            std::vector<VkQueueFamilyProperties> queue_family_props(num_queue_families);
            vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &num_queue_families, &queue_family_props[0]);

            indices.graphics_family.reset();
            for (uint32_t i = 0; i < queue_family_props.size(); ++i)
            {
                const VkQueueFamilyProperties &props = queue_family_props[i];
                if (props.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                    indices.graphics_family = i;
                
                VkBool32 present_support = VK_FALSE;
                vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &present_support);
                if (present_support)
                    indices.present_family = i;
                
                if (indices.IsComplete())
                    break;
            }
        }

        void GetSwapchainDetails(VkPhysicalDevice physical_device, VkSurfaceKHR surface, VulkanSwapchainDetails &details)
        {
            std::memset(&details, 0, sizeof(details));
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &details.capabilities);

            uint32_t num_formats;
            vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &num_formats, nullptr);
            details.formats.resize(num_formats);
            vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &num_formats, &details.formats[0]);

            uint32_t num_present_modes;
            vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &num_present_modes, nullptr);
            details.present_modes.resize(num_present_modes);
            vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &num_present_modes, &details.present_modes[0]);
        }

        uint32_t GetVulkanMemoryType(VkPhysicalDevice physical_device, uint32_t filter, VkMemoryPropertyFlags flags)
        {
            VkPhysicalDeviceMemoryProperties props;
            vkGetPhysicalDeviceMemoryProperties(physical_device, &props);

            for (uint32_t i = 0; i < props.memoryTypeCount; ++i)
                if (filter & (1 << i) && (props.memoryTypes[i].propertyFlags & flags) == flags)
                    return i;
            
            throw std::runtime_error("Error getting Vulkan physical device memory type: No suitable memory type found");
        }

        void CreateVulkanInstance(uint32_t num_extensions, const char *const *extensions, VulkanData *dst, uint32_t &version_major, uint32_t &version_minor)
        {
            // Get available extensions
            uint32_t num_available_extensions;
            vkEnumerateInstanceExtensionProperties(nullptr, &num_available_extensions, nullptr);
            std::vector<VkExtensionProperties> available_extensions(num_available_extensions);
            vkEnumerateInstanceExtensionProperties(nullptr, &num_available_extensions, &available_extensions[0]);

            // Get available layers
            uint32_t num_available_layers;
            vkEnumerateInstanceLayerProperties(&num_available_layers, nullptr);
            std::vector<VkLayerProperties> available_layers(num_available_layers);
            vkEnumerateInstanceLayerProperties(&num_available_layers, &available_layers[0]);

            std::vector<const char*> necessary_extensions(extensions, extensions + num_extensions);
            necessary_extensions.insert(necessary_extensions.end(), REQUIRED_EXTENSIONS.begin(), REQUIRED_EXTENSIONS.end());
            for (const char *required_extension : necessary_extensions)
            {
                bool found = false;
                for (const VkExtensionProperties &extension : available_extensions)
                {
                    if (strcmp(required_extension, extension.extensionName) == 0)
                    {
                        found = true;
                        break;
                    }
                }

                if (!found)
                    throw std::runtime_error(std::string("Error creating Vulkan context. Requested/Required extension '") + required_extension + "' not available");
            }

            for (const char *required_layer : REQUIRED_LAYERS)
            {
                bool found = false;
                for (const VkLayerProperties &layer : available_layers)
                {
                    if (strcmp(required_layer, layer.layerName) == 0)
                    {
                        found = true;
                        break;
                    }
                }

                if (!found)
                    throw std::runtime_error(std::string("Error creating Vulkan context. Requested layer '") + required_layer + "' not available");
            }

            // Create instance
            VkApplicationInfo app_info{};
            app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            app_info.pApplicationName = "RUT Application";
            app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
            app_info.pEngineName = "No Engine";
            app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
            app_info.apiVersion = VK_API_VERSION_1_0;

            VkInstanceCreateInfo instance_create_info{};
            instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            instance_create_info.pApplicationInfo = &app_info;
            instance_create_info.enabledExtensionCount = necessary_extensions.size();
            instance_create_info.ppEnabledExtensionNames = necessary_extensions.data();
            instance_create_info.enabledLayerCount = REQUIRED_LAYERS.size();
            instance_create_info.ppEnabledLayerNames = REQUIRED_LAYERS.data();

#ifdef RUT_BUILD_DEBUG
            VkDebugUtilsMessengerCreateInfoEXT instance_debug_messenger_create_info{};
            SetVulkanDebugMessengerCreateInfo(instance_debug_messenger_create_info);
            instance_create_info.pNext = &instance_debug_messenger_create_info;
#endif

            if (vkCreateInstance(&instance_create_info, nullptr, &dst->instance) != VK_SUCCESS)
                throw std::runtime_error("Error creating Vulkan context: vkCreateInstance failed");
            
            uint32_t version;
            vkEnumerateInstanceVersion(&version);
            version_major = VK_VERSION_MAJOR(version);
            version_minor = VK_VERSION_MINOR(version);

#ifdef RUT_BUILD_DEBUG
            VkDebugUtilsMessengerCreateInfoEXT debug_messenger_create_info{};
            SetVulkanDebugMessengerCreateInfo(debug_messenger_create_info);

            if (CreateDebugUtilsMessengerEXT(dst->instance, &debug_messenger_create_info, nullptr, &dst->debug_messenger) != VK_SUCCESS)
                throw std::runtime_error("Error creating Vulkan context: vkCreateDebugUtilsMessengerEXT failed");
#endif
        }

        void SetupVulkanDevice(uint32_t num_extensions, const char *const *extensions, VulkanData *data)
        {
            std::vector<const char*> necessary_extensions(extensions, extensions + num_extensions);

            // Select physical device
            uint32_t num_physical_devices;
            vkEnumeratePhysicalDevices(data->instance, &num_physical_devices, nullptr);
            if (num_physical_devices == 0)
                throw std::runtime_error("Error creating Vulkan context: No physical devices available");
            std::vector<VkPhysicalDevice> physical_devices(num_physical_devices);
            vkEnumeratePhysicalDevices(data->instance, &num_physical_devices, &physical_devices[0]);

            auto GetPhysicalDeviceSuitability = [data, &necessary_extensions](VkPhysicalDevice physical_device)
            {
                VkPhysicalDeviceProperties props;
                vkGetPhysicalDeviceProperties(physical_device, &props);

                VkPhysicalDeviceFeatures features;
                vkGetPhysicalDeviceFeatures(physical_device, &features);

                uint32_t num_extensions;
                vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &num_extensions, nullptr);
                std::vector<VkExtensionProperties> extensions(num_extensions);
                vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &num_extensions, extensions.data());

                for (const char *necessary_extension : necessary_extensions)
                {
                    bool found = false;
                    for (const VkExtensionProperties &extension : extensions)
                    {
                        if (strcmp(necessary_extension, extension.extensionName) == 0)
                        {
                            found = true;
                            break;
                        }
                    }

                    if (!found)
                        return 0;
                }
                
                VulkanQueueFamilyIndices indices;
                GetVulkanQueueFamilies(physical_device, data->surface, indices);

                VulkanSwapchainDetails details;
                GetSwapchainDetails(physical_device, data->surface, details);

                if (!features.geometryShader || !indices.IsComplete() || !details.IsAdequate())
                    return 0;
                
                int score = 1;
                
                switch (props.deviceType)
                {
                    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                        score += 1000;
                        break;
                    
                    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                        score += 100;
                        break;
                }

                return score;
            };

            std::unordered_map<VkPhysicalDevice, int> dev_scores;
            for (VkPhysicalDevice dev : physical_devices)
                dev_scores[dev] = GetPhysicalDeviceSuitability(dev);

            data->physical_device = VK_NULL_HANDLE;
            int max_score = 0;
            for (const auto &entry : dev_scores)
            {
                if (entry.second > max_score)
                {
                    data->physical_device = entry.first;
                    max_score = entry.second;
                }
            }
            if (data->physical_device == VK_NULL_HANDLE)
                throw std::runtime_error("Error creating Vulkan context: No suitable physical devices available");
            
            // Queues
            VulkanQueueFamilyIndices queue_family_indices;
            GetVulkanQueueFamilies(data->physical_device, data->surface, queue_family_indices);

            std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
            std::set<uint32_t> unique_queue_families =
            {
                queue_family_indices.graphics_family.value(),
                queue_family_indices.present_family.value()
            };
            float queue_priority = 1.0f;

            for (uint32_t queue_family : unique_queue_families)
            {
                VkDeviceQueueCreateInfo queue_create_info{};
                queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queue_create_info.queueFamilyIndex = queue_family_indices.graphics_family.value();
                queue_create_info.queueCount = 1;
                queue_create_info.pQueuePriorities = &queue_priority;
                queue_create_infos.push_back(queue_create_info);
            }

            // Device
            VkPhysicalDeviceFeatures requested_features{};
            requested_features.geometryShader = VK_TRUE;

            VkDeviceCreateInfo device_create_info{};
            device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            device_create_info.queueCreateInfoCount = queue_create_infos.size();
            device_create_info.pQueueCreateInfos = queue_create_infos.data();
            device_create_info.pEnabledFeatures = &requested_features;
            device_create_info.enabledExtensionCount = necessary_extensions.size();
            device_create_info.ppEnabledExtensionNames = necessary_extensions.data();
            device_create_info.enabledLayerCount = REQUIRED_LAYERS.size();
            device_create_info.ppEnabledLayerNames = REQUIRED_LAYERS.data();

            if (vkCreateDevice(data->physical_device, &device_create_info, nullptr, &data->device) != VK_SUCCESS)
                throw std::runtime_error("Error creating Vulkan context: vkCreateDevice failed");
            
            vkGetDeviceQueue(data->device, queue_family_indices.graphics_family.value(), 0, &data->graphics_queue);
            vkGetDeviceQueue(data->device, queue_family_indices.present_family.value(), 0, &data->present_queue);
        }

        void SetupVulkanSyncObjects(VulkanData *data)
        {
            // Setup sync objects
            VkSemaphoreCreateInfo sem_create_info{};
            sem_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

            VkFenceCreateInfo fence_create_info{};
            fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

            data->image_available_sems.resize(MAX_FRAMES_IN_FLIGHT);
            data->render_finished_sems.resize(MAX_FRAMES_IN_FLIGHT);
            data->in_flight_fences.resize(MAX_FRAMES_IN_FLIGHT);
            for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
            {
                if (vkCreateSemaphore(data->device, &sem_create_info, nullptr, &data->image_available_sems[i]) != VK_SUCCESS
                    || vkCreateSemaphore(data->device, &sem_create_info, nullptr, &data->render_finished_sems[i]) != VK_SUCCESS
                    || vkCreateFence(data->device, &fence_create_info, nullptr, &data->in_flight_fences[i]) != VK_SUCCESS)
                    throw std::runtime_error("Error creating Vulkan context: Failed to create sync objects");
            }
        }

        void SetupVulkanSwapchain(uint32_t width, uint32_t height, VulkanData *data)
        {
            vkDeviceWaitIdle(data->device);

            // Clean up previous
            VkSwapchainKHR old_swapchain = nullptr;
            if (data->have_swapchain)
            {
                old_swapchain = data->swapchain;

                vkDestroyCommandPool(data->device, data->cmd_pool, nullptr);

                for (VkFramebuffer framebuffer : data->swapchain_framebuffers)
                    vkDestroyFramebuffer(data->device, framebuffer, nullptr);
                
                for (VkImageView image_view : data->swapchain_image_views)
                    vkDestroyImageView(data->device, image_view, nullptr);

                vkDestroyRenderPass(data->device, data->render_pass, nullptr);
            }

            // Create new swapchain
            data->have_swapchain = true;
            VulkanSwapchainDetails swapchain_details;
            GetSwapchainDetails(data->physical_device, data->surface, swapchain_details);

            VkSurfaceFormatKHR surface_format = swapchain_details.ChooseFormat();
            VkPresentModeKHR present_mode = swapchain_details.ChoosePresentMode();
            VkExtent2D extent = swapchain_details.ChooseExtent(width, height);

            uint32_t image_count  = swapchain_details.capabilities.minImageCount + 1;
            if (swapchain_details.capabilities.maxImageCount > 0 && image_count > swapchain_details.capabilities.maxImageCount)
                image_count = swapchain_details.capabilities.maxImageCount;
            
            VkSwapchainCreateInfoKHR swapchain_create_info{};
            swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            swapchain_create_info.surface = data->surface;
            swapchain_create_info.minImageCount = image_count;
            swapchain_create_info.imageFormat = surface_format.format;
            swapchain_create_info.imageColorSpace = surface_format.colorSpace;
            swapchain_create_info.imageExtent = extent;
            swapchain_create_info.imageArrayLayers = 1;
            swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

            VulkanQueueFamilyIndices indices;
            GetVulkanQueueFamilies(data->physical_device, data->surface, indices);
            uint32_t queue_family_indices[] =
            {
                indices.graphics_family.value(),
                indices.present_family.value()
            };

            if (indices.graphics_family != indices.present_family)
            {
                swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
                swapchain_create_info.queueFamilyIndexCount = 2;
                swapchain_create_info.pQueueFamilyIndices = queue_family_indices;
            }
            else
            {
                swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
                swapchain_create_info.queueFamilyIndexCount = 0;
                swapchain_create_info.pQueueFamilyIndices = nullptr;
            }

            swapchain_create_info.preTransform = swapchain_details.capabilities.currentTransform;
            swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            swapchain_create_info.presentMode = present_mode;
            swapchain_create_info.clipped = VK_TRUE;
            swapchain_create_info.oldSwapchain = old_swapchain;

            // Create new swapchain
            if (vkCreateSwapchainKHR(data->device, &swapchain_create_info, nullptr, &data->swapchain) != VK_SUCCESS)
                throw std::runtime_error("Error creating Vulkan context: vkCreateSwapchainKHR failed");
            
            // Destroy old swapchain
            if (old_swapchain)
                vkDestroySwapchainKHR(data->device, old_swapchain, nullptr);

            data->swapchain_format = surface_format.format;
            data->swapchain_extent = extent;

            vkGetSwapchainImagesKHR(data->device, data->swapchain, &image_count, nullptr);
            data->swapchain_images.resize(image_count);
            vkGetSwapchainImagesKHR(data->device, data->swapchain, &image_count, &data->swapchain_images[0]);

            // Setup the render pass
            VkAttachmentDescription color_attachment_desc{};
            color_attachment_desc.format = surface_format.format;
            color_attachment_desc.samples = VK_SAMPLE_COUNT_1_BIT;
            color_attachment_desc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            color_attachment_desc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            color_attachment_desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            color_attachment_desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            color_attachment_desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            color_attachment_desc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            VkAttachmentReference color_attachment_ref{};
            color_attachment_ref.attachment = 0;
            color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            VkSubpassDescription subpass_desc{};
            subpass_desc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass_desc.colorAttachmentCount = 1;
            subpass_desc.pColorAttachments = &color_attachment_ref;

            VkSubpassDependency subpass_dependency{};
            subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            subpass_dependency.dstSubpass = 0;
            subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            subpass_dependency.srcAccessMask = 0;
            subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

            VkRenderPassCreateInfo render_pass_create_info{};
            render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            render_pass_create_info.attachmentCount = 1;
            render_pass_create_info.pAttachments = &color_attachment_desc;
            render_pass_create_info.subpassCount = 1;
            render_pass_create_info.pSubpasses = &subpass_desc;
            render_pass_create_info.dependencyCount = 1;
            render_pass_create_info.pDependencies = &subpass_dependency;

            if (vkCreateRenderPass(data->device, &render_pass_create_info, nullptr, &data->render_pass) != VK_SUCCESS)
                throw std::runtime_error("Error creating Vulkan context: vkCreateRenderPass failed");

            // Setup image views and framebuffers
            data->swapchain_image_views.reserve(image_count);
            data->swapchain_framebuffers.reserve(image_count);
            data->swapchain_image_views.clear();
            data->swapchain_framebuffers.clear();
            for (VkImage image : data->swapchain_images)
            {
                VkImageViewCreateInfo image_view_create_info{};
                image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
                image_view_create_info.format = data->swapchain_format;
                image_view_create_info.image = image;
                image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
                image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
                image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
                image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
                image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                image_view_create_info.subresourceRange.baseMipLevel = 0;
                image_view_create_info.subresourceRange.levelCount = 1;
                image_view_create_info.subresourceRange.baseArrayLayer = 0;
                image_view_create_info.subresourceRange.layerCount = 1;

                VkImageView image_view;
                if (vkCreateImageView(data->device, &image_view_create_info, nullptr, &image_view) != VK_SUCCESS)
                    throw std::runtime_error("Error creating Vulkan context: vkCreateImageView failed");
                data->swapchain_image_views.push_back(image_view);

                VkImageView attachments[] = { image_view };

                VkFramebufferCreateInfo framebuffer_create_info{};
                framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                framebuffer_create_info.renderPass = data->render_pass;
                framebuffer_create_info.attachmentCount = 1;
                framebuffer_create_info.pAttachments = attachments;
                framebuffer_create_info.width = extent.width;
                framebuffer_create_info.height = extent.height;
                framebuffer_create_info.layers = 1;

                VkFramebuffer framebuffer;
                if (vkCreateFramebuffer(data->device, &framebuffer_create_info, nullptr, &framebuffer) != VK_SUCCESS)
                    throw std::runtime_error("Error creating Vulkan context: vkCreateFramebuffer failed");
                data->swapchain_framebuffers.push_back(framebuffer);
            }

            // Setup command pool & buffer
            VkCommandPoolCreateInfo cmd_pool_create_info{};
            cmd_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            cmd_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            cmd_pool_create_info.queueFamilyIndex = indices.graphics_family.value();

            if (vkCreateCommandPool(data->device, &cmd_pool_create_info, nullptr, &data->cmd_pool) != VK_SUCCESS)
                throw std::runtime_error("Error creating Vulkan context: vkCreateCommandPool failed");
            
            VkCommandBufferAllocateInfo cmd_buffer_alloc_info{};
            cmd_buffer_alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            cmd_buffer_alloc_info.commandPool = data->cmd_pool;
            cmd_buffer_alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            cmd_buffer_alloc_info.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

            data->cmd_buffers.resize(MAX_FRAMES_IN_FLIGHT);
            if (vkAllocateCommandBuffers(data->device, &cmd_buffer_alloc_info, &data->cmd_buffers[0]) != VK_SUCCESS)
                throw std::runtime_error("Error creating Vulkan context: vkAllocateCommandBuffers failed");
            
            data->swapchain_renderable = true;
        }

        void BeginVulkanContext(VulkanData *data)
        {
            vkWaitForFences(data->device, 1, &data->in_flight_fences[data->current_frame], VK_TRUE, UINT64_MAX);
            vkResetFences(data->device, 1, &data->in_flight_fences[data->current_frame]);
            vkResetCommandBuffer(data->cmd_buffers[data->current_frame], 0);

            // Get next swapchain image
            VkResult result = vkAcquireNextImageKHR(data->device, data->swapchain, UINT64_MAX, data->image_available_sems[data->current_frame], VK_NULL_HANDLE, &data->current_image_index);
            if (result == VK_ERROR_OUT_OF_DATE_KHR)
                data->swapchain_renderable = false;
            else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
                throw std::runtime_error("Error beginning Vulkan context: vkAquireNextImageKHR failed");
        }

        void EndVulkanContext(VulkanData *data)
        {
            if (!data->swapchain_renderable)
                return;
            
            VkSubmitInfo submit_info{};
            submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

            VkSemaphore wait_semaphores[] = { data->image_available_sems[data->current_frame] };
            VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
            submit_info.waitSemaphoreCount = 1;
            submit_info.pWaitSemaphores = wait_semaphores;
            submit_info.pWaitDstStageMask = wait_stages;

            submit_info.commandBufferCount = 1;
            submit_info.pCommandBuffers = &data->cmd_buffers[data->current_frame];

            VkSemaphore signal_semaphores[] = { data->render_finished_sems[data->current_frame] };
            submit_info.signalSemaphoreCount = 1;
            submit_info.pSignalSemaphores = signal_semaphores;

            if (vkQueueSubmit(data->graphics_queue, 1, &submit_info, data->in_flight_fences[data->current_frame]) != VK_SUCCESS)
                throw std::runtime_error("Error creating Vulkan context: vkAllocateCommandBuffers failed");

            VkPresentInfoKHR present_info{};
            present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            present_info.waitSemaphoreCount = 1;
            present_info.pWaitSemaphores = signal_semaphores;
            present_info.swapchainCount = 1;
            present_info.pSwapchains = &data->swapchain;
            present_info.pImageIndices = &data->current_image_index;
            present_info.pResults = nullptr; // Optional

            VkResult result = vkQueuePresentKHR(data->present_queue, &present_info);
            if (result == VK_ERROR_OUT_OF_DATE_KHR)
                data->swapchain_renderable = false;
            if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
                throw std::runtime_error("Error swaping Vulkan buffers: vkQueuePresentKHR failed");

            data->current_frame = (data->current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
        }

        void DestroyVulkanInstance(VulkanData *data)
        {
            vkDeviceWaitIdle(data->device);

            if (data->have_swapchain)
            {
                vkDestroyCommandPool(data->device, data->cmd_pool, nullptr);

                for (VkFramebuffer framebuffer : data->swapchain_framebuffers)
                    vkDestroyFramebuffer(data->device, framebuffer, nullptr);
                
                for (VkImageView image_view : data->swapchain_image_views)
                    vkDestroyImageView(data->device, image_view, nullptr);

                vkDestroyRenderPass(data->device, data->render_pass, nullptr);
                vkDestroySwapchainKHR(data->device, data->swapchain, nullptr);
            }

            for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
            {
                vkDestroyFence(data->device, data->in_flight_fences[i], nullptr);
                vkDestroySemaphore(data->device, data->image_available_sems[i], nullptr);
                vkDestroySemaphore(data->device, data->render_finished_sems[i], nullptr);
            }

            vkDestroyDevice(data->device, nullptr);
            vkDestroySurfaceKHR(data->instance, data->surface, nullptr);

#ifdef RUT_BUILD_DEBUG
            DestroyDebugUtilsMessengerEXT(data->instance, data->debug_messenger, nullptr);
#endif

            vkDestroyInstance(data->instance, nullptr);
        }
    }
}

#endif