#pragma once

#include"RUT/Config.h"

#ifdef RUT_HAS_VULKAN

#include"RUT/Shader.h"
#include"VulkanUtils.h"

#include<vulkan/vulkan.h>

namespace rut
{
    namespace impl
    {
        class VulkanShaderUnit : public ShaderUnit
        {
        public:
            VulkanShaderUnit(Context *context, ShaderType type, const std::string &source);
            virtual ~VulkanShaderUnit();

            virtual uint64_t GetHandle() const override;
        
        private:
            VulkanData *m_data;
            VkShaderModule m_module;
        };

        class VulkanShaderProgram : public ShaderProgram
        {
        public:
            VulkanShaderProgram(Context *context, const ShaderProgramProperties &props);

            virtual const ShaderProgramProperties &GetProperties() const override;

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

            virtual void SetVariableArray(const std::string &name, size_t num_elements, const int *i) override;
            virtual void SetVariableArray(const std::string &name, size_t num_elements, const float *f) override;
            virtual void SetVariableArray(const std::string &name, size_t num_elements, const glm::ivec2 *v) override;
            virtual void SetVariableArray(const std::string &name, size_t num_elements, const glm::vec2 *v) override;
            virtual void SetVariableArray(const std::string &name, size_t num_elements, const glm::ivec3 *v) override;
            virtual void SetVariableArray(const std::string &name, size_t num_elements, const glm::vec3 *v) override;
            virtual void SetVariableArray(const std::string &name, size_t num_elements, const glm::ivec4 *v) override;
            virtual void SetVariableArray(const std::string &name, size_t num_elements, const glm::vec4 *v) override;
            virtual void SetVariableArray(const std::string &name, size_t num_elements, const glm::mat3 *m) override;
            virtual void SetVariableArray(const std::string &name, size_t num_elements, const glm::mat4 *m) override;
            
            virtual uint64_t GetHandle() const override;

            virtual const std::vector<VkPipelineShaderStageCreateInfo> &GetPipelineInfos() const;
        
        private:
            VulkanData *m_data;
            ShaderProgramProperties m_props;
            std::vector<VkPipelineShaderStageCreateInfo> m_pipeline_infos;
        };
    }
}

#endif