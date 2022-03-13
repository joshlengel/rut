#include"VulkanUniformBuffer.h"

#ifdef RUT_HAS_VULKAN

#include"RUT/Context.h"

#include<stdexcept>
#include<algorithm>
#include<cstring>

static const uint32_t VERTEX_TYPE_BYTES[] =
{
    4, 4, 8, 8, 12, 12, 16, 16, 44, 64
};

static const uint32_t VERTEX_TYPE_COUNTS[] =
{
    1, 1, 2, 2, 3, 3, 4, 4, 9, 16
};

namespace rut
{
    namespace impl
    {
        VulkanUniformBuffer::VulkanUniformBuffer(Context *context, const UniformLayout &layout):
            m_layout(layout)
        { Init(context); }

        VulkanUniformBuffer::VulkanUniformBuffer(Context *context, UniformLayout &&layout):
            m_layout(std::move(layout))
        { Init(context); }

        void VulkanUniformBuffer::Init(Context *context)
        {
            m_data = reinterpret_cast<VulkanData*>(context->GetHandle());

            // Setup layout
            uint32_t offset = 0;
            for (auto &item : m_layout)
            {
                uint32_t alignment;
                switch (item->GetType())
                {
                    case VT_INT:
                    case VT_FLOAT:
                        alignment = 4;
                        break;
                    
                    case VT_IVEC2:
                    case VT_FVEC2:
                        alignment = 8;
                        break;

                    case VT_IVEC3:
                    case VT_FVEC3:
                    case VT_IVEC4:
                    case VT_FVEC4:
                    case VT_MAT3:
                    case VT_MAT4:
                        alignment = 16;
                        break;
                }

                // Alignment
                uint32_t low_boundary = (offset / alignment) * alignment;
                if (low_boundary < offset)
                {
                    // Add padding
                    offset = low_boundary + alignment;
                }

                item->SetOffset(offset);
                item->SetSize(VERTEX_TYPE_BYTES[item->GetType()]);
                if (item->GetLength() > 1)
                {
                    dynamic_cast<UniformArrayLayoutItem*>(item)->SetStride(alignment);
                    item->SetSize(alignment * item->GetLength());
                }
                if (item->GetType() == VT_MAT3 || item->GetType() == VT_MAT4)
                {
                    UniformMatrixLayoutItem *mat_item = dynamic_cast<UniformMatrixLayoutItem*>(item);
                    mat_item->SetStride(4 * sizeof(float));
                }

                offset += item->GetSize();
            }
            m_layout.SetStride(offset);

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

        const UniformLayout &VulkanUniformBuffer::GetLayout() const { return m_layout; }

        void VulkanUniformBuffer::Map() { vkMapMemory(m_data->device, m_buffer_data.memorys[m_data->current_frame], 0, m_layout.GetStride(), 0, &m_mapped_data); }
        void VulkanUniformBuffer::Unmap() { vkUnmapMemory(m_data->device, m_buffer_data.memorys[m_data->current_frame]); }
        
#define SET_UNIFORM(name, value)\
auto itr = std::find_if(m_layout.begin(), m_layout.end(), [&](LayoutItem *item){ return item->GetName() == name; });\
if (itr == m_layout.end()) return;\
(*itr)->Write(reinterpret_cast<uint8_t*>(m_mapped_data) + (*itr)->GetOffset(), value);

        void VulkanUniformBuffer::SetVariable(const std::string &name, int i)               { SET_UNIFORM(name, &i); }
        void VulkanUniformBuffer::SetVariable(const std::string &name, float f)             { SET_UNIFORM(name, &f); }
        void VulkanUniformBuffer::SetVariable(const std::string &name, const glm::ivec2 &v) { SET_UNIFORM(name, &v); }
        void VulkanUniformBuffer::SetVariable(const std::string &name, const glm::vec2 &v)  { SET_UNIFORM(name, &v); }
        void VulkanUniformBuffer::SetVariable(const std::string &name, const glm::ivec3 &v) { SET_UNIFORM(name, &v); }
        void VulkanUniformBuffer::SetVariable(const std::string &name, const glm::vec3 &v)  { SET_UNIFORM(name, &v); }
        void VulkanUniformBuffer::SetVariable(const std::string &name, const glm::ivec4 &v) { SET_UNIFORM(name, &v); }
        void VulkanUniformBuffer::SetVariable(const std::string &name, const glm::vec4 &v)  { SET_UNIFORM(name, &v); }
        void VulkanUniformBuffer::SetVariable(const std::string &name, const glm::mat3 &m)  { SET_UNIFORM(name, &m); }
        void VulkanUniformBuffer::SetVariable(const std::string &name, const glm::mat4 &m)  { SET_UNIFORM(name, &m); }
        void VulkanUniformBuffer::SetVariable(const std::string &name, const int *i)        { SET_UNIFORM(name, i); }
        void VulkanUniformBuffer::SetVariable(const std::string &name, const float *f)      { SET_UNIFORM(name, f); }
        void VulkanUniformBuffer::SetVariable(const std::string &name, const glm::ivec2 *v) { SET_UNIFORM(name, v); }
        void VulkanUniformBuffer::SetVariable(const std::string &name, const glm::vec2 *v)  { SET_UNIFORM(name, v); }
        void VulkanUniformBuffer::SetVariable(const std::string &name, const glm::ivec3 *v) { SET_UNIFORM(name, v); }
        void VulkanUniformBuffer::SetVariable(const std::string &name, const glm::vec3 *v)  { SET_UNIFORM(name, v); }
        void VulkanUniformBuffer::SetVariable(const std::string &name, const glm::ivec4 *v) { SET_UNIFORM(name, v); }
        void VulkanUniformBuffer::SetVariable(const std::string &name, const glm::vec4 *v)  { SET_UNIFORM(name, v); }
        void VulkanUniformBuffer::SetVariable(const std::string &name, const glm::mat3 *m)  { SET_UNIFORM(name, m); }
        void VulkanUniformBuffer::SetVariable(const std::string &name, const glm::mat4 *m)  { SET_UNIFORM(name, m); }

        uint64_t VulkanUniformBuffer::GetHandle() const { return reinterpret_cast<uint64_t>(&m_buffer_data); }
    }
}

#endif