#include"VulkanMesh.h"

#ifdef RUT_HAS_VULKAN

#include"RUT/Context.h"

#include<stdexcept>
#include<cstring>

namespace rut
{
    namespace impl
    {
        VulkanMesh::VulkanMesh(Context *context, const VertexLayout &layout):
            m_data(reinterpret_cast<VulkanData*>(context->GetHandle())),
            m_layout(layout)
        { Init(); }

        VulkanMesh::VulkanMesh(Context *context, VertexLayout &&layout):
            m_data(reinterpret_cast<VulkanData*>(context->GetHandle())),
            m_layout(std::move(layout))
        { Init(); }

        void VulkanMesh::Init()
        {}

        VulkanMesh::~VulkanMesh()
        {
            vkDeviceWaitIdle(m_data->device);
            
            if (m_mesh_data.have_buffers[0])
            {
                vkDestroyBuffer(m_data->device, m_mesh_data.buffers[0], nullptr);
                vkFreeMemory(m_data->device, m_mesh_data.mems[0], nullptr);
            }

            if (m_mesh_data.have_buffers[1])
            {
                vkDestroyBuffer(m_data->device, m_mesh_data.buffers[1], nullptr);
                vkFreeMemory(m_data->device, m_mesh_data.mems[1], nullptr);
            }
        }

        const VertexLayout &VulkanMesh::GetLayout() const { return m_layout; }

        void VulkanMesh::SetVertices(size_t num_vertices, const void *vertices)
        {
            if (m_mesh_data.have_buffers[0])
            {
                vkDestroyBuffer(m_data->device, m_mesh_data.buffers[0], nullptr);
                vkFreeMemory(m_data->device, m_mesh_data.mems[0], nullptr);
            }

            m_mesh_data.have_buffers[0] = true;

            VkBufferCreateInfo buffer_create_info{};
            buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            buffer_create_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            buffer_create_info.size = num_vertices * m_layout.GetStride();
            buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            if (vkCreateBuffer(m_data->device, &buffer_create_info, nullptr, &m_mesh_data.buffers[0]) != VK_SUCCESS)
                throw std::runtime_error("Error setting Vulkan mesh vertices: vkCreateBuffer failed");
            
            VkMemoryRequirements mem_requirements;
            vkGetBufferMemoryRequirements(m_data->device, m_mesh_data.buffers[0], &mem_requirements);

            VkMemoryAllocateInfo alloc_info{};
            alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            alloc_info.allocationSize = mem_requirements.size;
            alloc_info.memoryTypeIndex = GetVulkanMemoryType(m_data->physical_device, mem_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

            if (vkAllocateMemory(m_data->device, &alloc_info, nullptr, &m_mesh_data.mems[0]) != VK_SUCCESS)
                throw std::runtime_error("Error setting Vulkan mesh vertices: vkAllocateMemory failed");
            
            vkBindBufferMemory(m_data->device, m_mesh_data.buffers[0], m_mesh_data.mems[0], 0);

            // Load data
            void *data;
            vkMapMemory(m_data->device, m_mesh_data.mems[0], 0, buffer_create_info.size, 0, &data);
            std::memcpy(data, vertices, buffer_create_info.size);
            vkUnmapMemory(m_data->device, m_mesh_data.mems[0]);

            m_mesh_data.num_vertices = num_vertices;
        }

        void VulkanMesh::SetIndices(size_t num_indices, const uint32_t *indices)
        {
            if (m_mesh_data.have_buffers[1])
            {
                vkDestroyBuffer(m_data->device, m_mesh_data.buffers[1], nullptr);
                vkFreeMemory(m_data->device, m_mesh_data.mems[1], nullptr);
            }

            m_mesh_data.have_buffers[1] = true;

            VkBufferCreateInfo buffer_create_info{};
            buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            buffer_create_info.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
            buffer_create_info.size = num_indices * sizeof(uint32_t);
            buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            if (vkCreateBuffer(m_data->device, &buffer_create_info, nullptr, &m_mesh_data.buffers[1]) != VK_SUCCESS)
                throw std::runtime_error("Error setting Vulkan mesh indices: vkCreateBuffer failed");
            
            VkMemoryRequirements mem_requirements;
            vkGetBufferMemoryRequirements(m_data->device, m_mesh_data.buffers[1], &mem_requirements);

            VkMemoryAllocateInfo alloc_info{};
            alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            alloc_info.allocationSize = mem_requirements.size;
            alloc_info.memoryTypeIndex = GetVulkanMemoryType(m_data->physical_device, mem_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

            if (vkAllocateMemory(m_data->device, &alloc_info, nullptr, &m_mesh_data.mems[1]) != VK_SUCCESS)
                throw std::runtime_error("Error setting Vulkan mesh indices: vkAllocateMemory failed");
            
            vkBindBufferMemory(m_data->device, m_mesh_data.buffers[1], m_mesh_data.mems[1], 0);

            // Load data
            void *data;
            vkMapMemory(m_data->device, m_mesh_data.mems[1], 0, buffer_create_info.size, 0, &data);
            std::memcpy(data, indices, buffer_create_info.size);
            vkUnmapMemory(m_data->device, m_mesh_data.mems[1]);

            m_mesh_data.num_indices = num_indices;
        }

        uint64_t VulkanMesh::GetHandle() const { return reinterpret_cast<uint64_t>(&m_mesh_data); }
    }
}

#endif