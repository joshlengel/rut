#include"VulkanShader.h"

#ifdef RUT_HAS_VULKAN

#include"RUT/Context.h"
#include"ShaderTools.h"
#include"impl/Vulkan/VulkanUtils.h"

#include<stdexcept>

namespace rut
{
    namespace impl
    {
        VulkanShaderUnit::VulkanShaderUnit(Context *context, ShaderType type, const std::string &source):
            m_data(reinterpret_cast<VulkanData*>(context->GetHandle()))
        {
            CodeBlob blob = GenerateVulkanShader(source, type);

            // Create vulkan shader module
            VkShaderModuleCreateInfo create_info{};
            create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            create_info.codeSize = blob.bytes;
            create_info.pCode = reinterpret_cast<uint32_t*>(blob.data);

            if (vkCreateShaderModule(m_data->device, &create_info, nullptr, &m_module) != VK_SUCCESS)
                throw std::runtime_error("Error creating Vulkan shader: vkCreateShaderModule failed");
            
            delete[] blob.data;
        }

        VulkanShaderUnit::~VulkanShaderUnit()
        {
            vkDestroyShaderModule(m_data->device, m_module, nullptr);
        }

        uint64_t VulkanShaderUnit::GetHandle() const { return reinterpret_cast<uint64_t>(m_module); }

        VulkanShaderProgram::VulkanShaderProgram(Context *context, const ShaderProgramProperties &props):
            m_data(reinterpret_cast<VulkanData*>(context->GetHandle())),
            m_props(props)
        {
            auto AddPipelineInfo = [&](VkShaderStageFlagBits bits, std::shared_ptr<ShaderUnit> unit)
            {
                VkPipelineShaderStageCreateInfo create_info{};
                create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                create_info.stage = bits;
                create_info.module = reinterpret_cast<VkShaderModule>(unit->GetHandle());
                create_info.pName = "main";
                m_pipeline_infos.push_back(create_info);
            };

            if (m_props.vertex_shader)
                AddPipelineInfo(VK_SHADER_STAGE_VERTEX_BIT, m_props.vertex_shader);
            
            if (m_props.fragment_shader)
                AddPipelineInfo(VK_SHADER_STAGE_FRAGMENT_BIT, m_props.fragment_shader);
            
            if (m_props.geometry_shader)
                AddPipelineInfo(VK_SHADER_STAGE_GEOMETRY_BIT, m_props.geometry_shader);
        }

        const ShaderProgramProperties &VulkanShaderProgram::GetProperties() const { return m_props; }

        void VulkanShaderProgram::Map() {}
        void VulkanShaderProgram::Unmap() {}

        void VulkanShaderProgram::SetVariable(const std::string &name, int i) {}
        void VulkanShaderProgram::SetVariable(const std::string &name, float f) {}
        void VulkanShaderProgram::SetVariable(const std::string &name, const glm::ivec2 &v) {}
        void VulkanShaderProgram::SetVariable(const std::string &name, const glm::vec2 &v) {}
        void VulkanShaderProgram::SetVariable(const std::string &name, const glm::ivec3 &v) {}
        void VulkanShaderProgram::SetVariable(const std::string &name, const glm::vec3 &v) {}
        void VulkanShaderProgram::SetVariable(const std::string &name, const glm::ivec4 &v) {}
        void VulkanShaderProgram::SetVariable(const std::string &name, const glm::vec4 &v) {}
        void VulkanShaderProgram::SetVariable(const std::string &name, const glm::mat3 &m) {}
        void VulkanShaderProgram::SetVariable(const std::string &name, const glm::mat4 &m) {}

        void VulkanShaderProgram::SetVariableArray(const std::string &name, size_t num_elements, const int *i) {}
        void VulkanShaderProgram::SetVariableArray(const std::string &name, size_t num_elements, const float *f) {}
        void VulkanShaderProgram::SetVariableArray(const std::string &name, size_t num_elements, const glm::ivec2 *v) {}
        void VulkanShaderProgram::SetVariableArray(const std::string &name, size_t num_elements, const glm::vec2 *v) {}
        void VulkanShaderProgram::SetVariableArray(const std::string &name, size_t num_elements, const glm::ivec3 *v) {}
        void VulkanShaderProgram::SetVariableArray(const std::string &name, size_t num_elements, const glm::vec3 *v) {}
        void VulkanShaderProgram::SetVariableArray(const std::string &name, size_t num_elements, const glm::ivec4 *v) {}
        void VulkanShaderProgram::SetVariableArray(const std::string &name, size_t num_elements, const glm::vec4 *v) {}
        void VulkanShaderProgram::SetVariableArray(const std::string &name, size_t num_elements, const glm::mat3 *m) {}
        void VulkanShaderProgram::SetVariableArray(const std::string &name, size_t num_elements, const glm::mat4 *m) {}
    
        uint64_t VulkanShaderProgram::GetHandle() const { return 0; }

        const std::vector<VkPipelineShaderStageCreateInfo> &VulkanShaderProgram::GetPipelineInfos() const { return m_pipeline_infos; }
    }
}

#endif