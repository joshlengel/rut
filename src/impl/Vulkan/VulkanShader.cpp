#include"VulkanShader.h"

#ifdef RUT_HAS_VULKAN

#include"RUT/Context.h"
#include"ShaderTools.h"
#include"impl/Vulkan/VulkanUtils.h"

#include<stdexcept>
#include<cstring>
#include<algorithm>

static VkShaderStageFlagBits SHADER_TYPE_TO_STAGE_BITS[] =
{
    VK_SHADER_STAGE_VERTEX_BIT,
    VK_SHADER_STAGE_FRAGMENT_BIT,
    VK_SHADER_STAGE_GEOMETRY_BIT
};

namespace rut
{
    namespace impl
    {
        VulkanShaderUnit::VulkanShaderUnit(Context *context, ShaderType type, const std::string &source):
            m_data(reinterpret_cast<VulkanData*>(context->GetHandle())),
            m_type(type)
        {
            // Compile
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

        ShaderType VulkanShaderUnit::GetType() const { return m_type; }

        uint64_t VulkanShaderUnit::GetHandle() const { return reinterpret_cast<uint64_t>(m_module); }

        VulkanShaderProgram::VulkanShaderProgram(Context *context, const ShaderProgramCreateProperties &create_props):
            m_data(reinterpret_cast<VulkanData*>(context->GetHandle())),
            m_props(create_props.props)
        {
            //uint32_t uniform_buffer_index = 0;
            auto ProcessUnit = [&](VkShaderStageFlagBits bits, std::shared_ptr<ShaderUnit> unit)
            {
                VkPipelineShaderStageCreateInfo create_info{};
                create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                create_info.stage = bits;
                create_info.module = reinterpret_cast<VkShaderModule>(unit->GetHandle());
                create_info.pName = "main";
                m_pipeline_infos.push_back(create_info);
            };

            if (create_props.vertex_shader)
                ProcessUnit(VK_SHADER_STAGE_VERTEX_BIT, create_props.vertex_shader);
            
            if (create_props.fragment_shader)
                ProcessUnit(VK_SHADER_STAGE_FRAGMENT_BIT, create_props.fragment_shader);
            
            if (create_props.geometry_shader)
                ProcessUnit(VK_SHADER_STAGE_GEOMETRY_BIT, create_props.geometry_shader);
            
            for (const auto &uniform_binding : m_props.uniform_bindings)
            {
                VkDescriptorSetLayoutBinding binding{};
                binding.binding = uniform_binding.binding;
                binding.descriptorCount = 1;
                binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                binding.pImmutableSamplers = nullptr;
                binding.stageFlags = SHADER_TYPE_TO_STAGE_BITS[uniform_binding.stage];

                m_layout_bindings.push_back(binding);
            }
        }

        const ShaderProgramProperties &VulkanShaderProgram::GetProperties() const { return m_props; }
    
        void VulkanShaderProgram::BindUniformBuffer(uint32_t binding, std::shared_ptr<UniformBuffer> buffer) { m_bound_buffers[binding] = buffer; }

        uint64_t VulkanShaderProgram::GetHandle() const { return 0; }

        const std::vector<VkPipelineShaderStageCreateInfo> &VulkanShaderProgram::GetPipelineInfos() const { return m_pipeline_infos; }
        const std::vector<VkDescriptorSetLayoutBinding> &VulkanShaderProgram::GetLayoutBindings() const { return m_layout_bindings; }
        const std::unordered_map<uint32_t, std::shared_ptr<UniformBuffer>> &VulkanShaderProgram::GetBoundBuffers() const { return m_bound_buffers; }
    }
}

#endif