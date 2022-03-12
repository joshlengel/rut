#pragma once

#include"RUT/Config.h"

#ifdef RUT_HAS_VULKAN

#include"RUT/Shader.h"
#include"VulkanUtils.h"

#include<unordered_map>

namespace rut
{
    class UniformBuffer;

    namespace impl
    {
        struct StringPointerLess
        {
            bool operator()(const std::string *str1, const std::string *str2) const { return *str1 < *str2; }
        };

        class VulkanShaderUnit : public ShaderUnit
        {
        public:
            VulkanShaderUnit(Context *context, ShaderType type, const std::string &source);
            virtual ~VulkanShaderUnit();

            virtual ShaderType GetType() const override;

            virtual uint64_t GetHandle() const override;
        
        private:
            VulkanData *m_data;
            ShaderType m_type;
            VkShaderModule m_module;
        };

        class VulkanShaderProgram : public ShaderProgram
        {
        public:
            VulkanShaderProgram(Context *context, const ShaderProgramCreateProperties &create_props);

            virtual const ShaderProgramProperties &GetProperties() const override;
            
            virtual void BindUniformBuffer(uint32_t binding, std::shared_ptr<UniformBuffer> buffer) override;

            virtual uint64_t GetHandle() const override;

            const std::vector<VkPipelineShaderStageCreateInfo> &GetPipelineInfos() const;
            const std::vector<VkDescriptorSetLayoutBinding> &GetLayoutBindings() const;
            const std::unordered_map<uint32_t, std::shared_ptr<UniformBuffer>> &GetBoundBuffers() const;
        
        private:
            VulkanData *m_data;
            ShaderProgramProperties m_props;
            std::vector<VkPipelineShaderStageCreateInfo> m_pipeline_infos;
            std::vector<VkDescriptorSetLayoutBinding> m_layout_bindings;
            std::unordered_map<uint32_t, std::shared_ptr<UniformBuffer>> m_bound_buffers;
        };
    }
}

#endif