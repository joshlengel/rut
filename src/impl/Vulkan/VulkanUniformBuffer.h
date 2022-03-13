#pragma once

#include"RUT/Config.h"

#ifdef RUT_HAS_VULKAN

#include"RUT/UniformBuffer.h"
#include"RUT/Layout.h"
#include"VulkanUtils.h"

#include<vector>

namespace rut
{
    namespace impl
    {
        struct VulkanUniformBufferData
        {
            std::vector<VkBuffer> buffers;
            std::vector<VkDeviceMemory> memorys;
        };

        class VulkanUniformBuffer : public UniformBuffer
        {
        public:
            VulkanUniformBuffer(Context *context, const UniformLayout &layout);
            VulkanUniformBuffer(Context *context, UniformLayout &&layout);
            virtual ~VulkanUniformBuffer();

            virtual const UniformLayout &GetLayout() const override;

            virtual void Map() override;
            virtual void Unmap() override;
            virtual void SetVariable(const std::string &name, int i) override;
            virtual void SetVariable(const std::string &name, float f) override;
            virtual void SetVariable(const std::string &name, const glm::ivec2 &v) override;
            virtual void SetVariable(const std::string &name, const glm::vec2 &v) override;
            virtual void SetVariable(const std::string &name, const glm::ivec3 &v) override;
            virtual void SetVariable(const std::string &name, const glm::vec3 &v) override;
            virtual void SetVariable(const std::string &name, const glm::ivec4 &v) override;
            virtual void SetVariable(const std::string &name, const glm::vec4 &v) override;
            virtual void SetVariable(const std::string &name, const glm::mat3 &m) override;
            virtual void SetVariable(const std::string &name, const glm::mat4 &m) override;
            virtual void SetVariable(const std::string &name, const int *i) override;
            virtual void SetVariable(const std::string &name, const float *f) override;
            virtual void SetVariable(const std::string &name, const glm::ivec2 *v) override;
            virtual void SetVariable(const std::string &name, const glm::vec2 *v) override;
            virtual void SetVariable(const std::string &name, const glm::ivec3 *v) override;
            virtual void SetVariable(const std::string &name, const glm::vec3 *v) override;
            virtual void SetVariable(const std::string &name, const glm::ivec4 *v) override;
            virtual void SetVariable(const std::string &name, const glm::vec4 *v) override;
            virtual void SetVariable(const std::string &name, const glm::mat3 *m) override;
            virtual void SetVariable(const std::string &name, const glm::mat4 *m) override;

            virtual uint64_t GetHandle() const override;
        
        private:
            VulkanData *m_data;
            UniformLayout m_layout;
            VulkanUniformBufferData m_buffer_data;
            void *m_mapped_data;

            void Init(Context *context);
        };
    }
}

#endif