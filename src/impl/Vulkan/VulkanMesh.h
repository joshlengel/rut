#pragma once

#include"RUT/Config.h"

#ifdef RUT_HAS_VULKAN

#include"RUT/Mesh.h"
#include"RUT/Layout.h"
#include"VulkanUtils.h"

namespace rut
{
    namespace impl
    {
        struct VulkanMeshData
        {
            VkBuffer buffers[2];
            VkDeviceMemory mems[2];
            bool have_buffers[2] = { false, false };
            uint32_t num_vertices = 0;
            uint32_t num_indices = 0;
        };

        class VulkanMesh : public Mesh
        {
        public:
            VulkanMesh(Context *context, const VertexLayout &layout);
            VulkanMesh(Context *context, VertexLayout &&layout);
            virtual ~VulkanMesh();

            virtual const VertexLayout &GetLayout() const override;

            virtual void SetVertices(size_t num_vertices, const void *vertices) override;
            virtual void SetIndices(size_t num_indices, const uint32_t *indices) override;

            virtual uint64_t GetHandle() const override;

        private:
            VulkanData *m_data;
            VertexLayout m_layout;

            VulkanMeshData m_mesh_data;

            void Init();
        };
    }
}

#endif