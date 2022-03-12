#include"VulkanUniformBuffer.h"

#ifdef RUT_HAS_VULKAN

#include"RUT/Context.h"

#include<stdexcept>
#include<algorithm>
#include<cstring>

namespace rut
{
    namespace impl
    {
        VulkanUniformBuffer::VulkanUniformBuffer(Context *context, const VertexLayout &layout):
            m_layout(layout)
        { Init(context); }

        VulkanUniformBuffer::VulkanUniformBuffer(Context *context, VertexLayout &&layout):
            m_layout(std::move(layout))
        { Init(context); }

        void VulkanUniformBuffer::Init(Context *context)
        {
            m_data = reinterpret_cast<VulkanData*>(context->GetHandle());

            VulkanQueueFamilyIndices indices;
            GetVulkanQueueFamilies(m_data->physical_device, m_data->surface, indices);

            m_buffer_data.buffers.resize(MAX_FRAMES_IN_FLIGHT);
            m_buffer_data.memorys.resize(MAX_FRAMES_IN_FLIGHT);
            for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
            {
                VkBufferCreateInfo create_info{};
                create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                create_info.queueFamilyIndexCount = 1;
                create_info.pQueueFamilyIndices = &indices.graphics_family.value();
                create_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
                create_info.size = m_layout.GetStride();
                
                if (vkCreateBuffer(m_data->device, &create_info, nullptr, &m_buffer_data.buffers[i]) != VK_SUCCESS)
                    throw std::runtime_error("Error creating Vulkan uniform buffer: vkCreateBuffer failed");
                
                VkMemoryRequirements mem_requirements;
                vkGetBufferMemoryRequirements(m_data->device, m_buffer_data.buffers[i], &mem_requirements);

                VkMemoryAllocateInfo alloc_info{};
                alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                alloc_info.memoryTypeIndex = GetVulkanMemoryType(m_data->physical_device, mem_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
                alloc_info.allocationSize = mem_requirements.size;
                
                if (vkAllocateMemory(m_data->device, &alloc_info, nullptr, &m_buffer_data.memorys[i]) != VK_SUCCESS)
                    throw std::runtime_error("Error creating Vulkan uniform buffer: vkAllocateMemory failed");
                
                vkBindBufferMemory(m_data->device, m_buffer_data.buffers[i], m_buffer_data.memorys[i], 0);
            }
        }

        VulkanUniformBuffer::~VulkanUniformBuffer()
        {
            for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
            {
                vkFreeMemory(m_data->device, m_buffer_data.memorys[i], nullptr);
                vkDestroyBuffer(m_data->device, m_buffer_data.buffers[i], nullptr);
            }
        }

        const VertexLayout &VulkanUniformBuffer::GetLayout() const { return m_layout; }

        void VulkanUniformBuffer::Map() { vkMapMemory(m_data->device, m_buffer_data.memorys[m_data->current_frame], 0, m_layout.GetStride(), 0, &m_mapped_data); }
        void VulkanUniformBuffer::Unmap() { vkUnmapMemory(m_data->device, m_buffer_data.memorys[m_data->current_frame]); }
        
#define SET_UNIFORM(name, num_elements, value)\
auto itr = std::find_if(m_layout.begin(), m_layout.end(), [&](const VertexLayoutItem &item){ return item.name == name; });\
if (itr == m_layout.end()) return;\
std::memcpy(reinterpret_cast<char*>(m_mapped_data) + itr->offset, value, num_elements * sizeof(*value));

        void VulkanUniformBuffer::SetVariable(const std::string &name, int i)               { SET_UNIFORM(name, 1, &i); }
        void VulkanUniformBuffer::SetVariable(const std::string &name, float f)             { SET_UNIFORM(name, 1, &f); }
        void VulkanUniformBuffer::SetVariable(const std::string &name, const glm::ivec2 &v) { SET_UNIFORM(name, 1, &v); }
        void VulkanUniformBuffer::SetVariable(const std::string &name, const glm::vec2 &v)  { SET_UNIFORM(name, 1, &v); }
        void VulkanUniformBuffer::SetVariable(const std::string &name, const glm::ivec3 &v) { SET_UNIFORM(name, 1, &v); }
        void VulkanUniformBuffer::SetVariable(const std::string &name, const glm::vec3 &v)  { SET_UNIFORM(name, 1, &v); }
        void VulkanUniformBuffer::SetVariable(const std::string &name, const glm::ivec4 &v) { SET_UNIFORM(name, 1, &v); }
        void VulkanUniformBuffer::SetVariable(const std::string &name, const glm::vec4 &v)  { SET_UNIFORM(name, 1, &v); }
        void VulkanUniformBuffer::SetVariable(const std::string &name, const glm::mat3 &m)  { SET_UNIFORM(name, 1, &m); }
        void VulkanUniformBuffer::SetVariable(const std::string &name, const glm::mat4 &m)  { SET_UNIFORM(name, 1, &m); }
        void VulkanUniformBuffer::SetVariable(const std::string &name, uint32_t num_elements, const int *i)        { SET_UNIFORM(name, num_elements, i); }
        void VulkanUniformBuffer::SetVariable(const std::string &name, uint32_t num_elements, const float *f)      { SET_UNIFORM(name, num_elements, f); }
        void VulkanUniformBuffer::SetVariable(const std::string &name, uint32_t num_elements, const glm::ivec2 *v) { SET_UNIFORM(name, num_elements, v); }
        void VulkanUniformBuffer::SetVariable(const std::string &name, uint32_t num_elements, const glm::vec2 *v)  { SET_UNIFORM(name, num_elements, v); }
        void VulkanUniformBuffer::SetVariable(const std::string &name, uint32_t num_elements, const glm::ivec3 *v) { SET_UNIFORM(name, num_elements, v); }
        void VulkanUniformBuffer::SetVariable(const std::string &name, uint32_t num_elements, const glm::vec3 *v)  { SET_UNIFORM(name, num_elements, v); }
        void VulkanUniformBuffer::SetVariable(const std::string &name, uint32_t num_elements, const glm::ivec4 *v) { SET_UNIFORM(name, num_elements, v); }
        void VulkanUniformBuffer::SetVariable(const std::string &name, uint32_t num_elements, const glm::vec4 *v)  { SET_UNIFORM(name, num_elements, v); }
        void VulkanUniformBuffer::SetVariable(const std::string &name, uint32_t num_elements, const glm::mat3 *m)  { SET_UNIFORM(name, num_elements, m); }
        void VulkanUniformBuffer::SetVariable(const std::string &name, uint32_t num_elements, const glm::mat4 *m)  { SET_UNIFORM(name, num_elements, m); }

        uint64_t VulkanUniformBuffer::GetHandle() const { return reinterpret_cast<uint64_t>(&m_buffer_data); }
    }
}

#endif