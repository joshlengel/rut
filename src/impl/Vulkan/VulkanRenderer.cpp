#include"VulkanRenderer.h"

#ifdef RUT_HAS_VULKAN

#include"RUT/Context.h"
#include"VulkanShader.h"
#include"VulkanMesh.h"

#include<stdexcept>

static const VkCullModeFlagBits CULL_MODE_TO_BITS[] =
{
    VK_CULL_MODE_NONE,
    VK_CULL_MODE_FRONT_BIT,
    VK_CULL_MODE_BACK_BIT,
    VK_CULL_MODE_FRONT_AND_BACK
};

static const VkFormat VERTEX_TYPE_TO_VK_FORMAT[] =
{
    VK_FORMAT_R32_SINT,
    VK_FORMAT_R32_SFLOAT,
    VK_FORMAT_R32G32_SINT,
    VK_FORMAT_R32G32_SFLOAT,
    VK_FORMAT_R32G32B32_SINT,
    VK_FORMAT_R32G32B32_SFLOAT,
    VK_FORMAT_R32G32B32A32_SINT,
    VK_FORMAT_R32G32B32A32_SFLOAT
};

namespace rut
{
    namespace impl
    {
        VulkanRenderer::VulkanRenderer(Context *context, const RendererProperties &props):
            m_data(reinterpret_cast<VulkanData*>(context->GetHandle())),
            m_props(props),
            m_cached_extent(m_data->swapchain_extent)
        {
            SetupPipeline();
        }

        void VulkanRenderer::SetupPipeline()
        {
            if (m_have_pipline)
            {
                vkDestroyPipeline(m_data->device, m_pipeline, nullptr);
                vkDestroyPipelineLayout(m_data->device, m_layout, nullptr);
            }

            m_have_pipline = true;

            VkVertexInputBindingDescription input_binding_desc{};
            input_binding_desc.binding = 0;
            input_binding_desc.stride = m_props.shader->GetProperties().layout.GetStride();
            input_binding_desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            std::vector<VkVertexInputAttributeDescription> attribute_descs;
            uint32_t num_attributes = std::distance(m_props.shader->GetProperties().layout.begin(), m_props.shader->GetProperties().layout.end());
            attribute_descs.reserve(num_attributes);

            uint32_t index = 0;
            for (const auto &item : m_props.shader->GetProperties().layout)
            {
                VkVertexInputAttributeDescription attribute_desc{};
                attribute_desc.binding = 0;
                attribute_desc.location = index++;
                attribute_desc.offset = item.offset;
                attribute_desc.format = VERTEX_TYPE_TO_VK_FORMAT[item.type];
                attribute_descs.push_back(attribute_desc);
            }

            VkPipelineVertexInputStateCreateInfo vertex_input_create_info{};
            vertex_input_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertex_input_create_info.vertexBindingDescriptionCount = 1;
            vertex_input_create_info.pVertexBindingDescriptions = &input_binding_desc;
            vertex_input_create_info.vertexAttributeDescriptionCount = num_attributes;
            vertex_input_create_info.pVertexAttributeDescriptions = attribute_descs.data();

            VkPipelineInputAssemblyStateCreateInfo input_assembly_create_info{};
            input_assembly_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            input_assembly_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            input_assembly_create_info.primitiveRestartEnable = VK_FALSE;

            VkViewport viewport{};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = static_cast<float>(m_data->swapchain_extent.width);
            viewport.height = static_cast<float>(m_data->swapchain_extent.height);
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;

            VkRect2D scissor{};
            scissor.offset = { 0, 0 };
            scissor.extent = m_data->swapchain_extent;

            VkPipelineViewportStateCreateInfo viewport_create_info{};
            viewport_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewport_create_info.viewportCount = 1;
            viewport_create_info.pViewports = &viewport;
            viewport_create_info.scissorCount = 1;
            viewport_create_info.pScissors = &scissor;

            VkPipelineRasterizationStateCreateInfo rasterizer_create_info{};
            rasterizer_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterizer_create_info.depthClampEnable = VK_FALSE;
            rasterizer_create_info.rasterizerDiscardEnable = VK_FALSE;
            rasterizer_create_info.polygonMode = VK_POLYGON_MODE_FILL;
            rasterizer_create_info.lineWidth = 1.0f;
            rasterizer_create_info.cullMode = CULL_MODE_TO_BITS[m_props.cull_mode];
            rasterizer_create_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
            rasterizer_create_info.depthBiasEnable = VK_FALSE;
            rasterizer_create_info.depthBiasConstantFactor = 0.0f; // Optional
            rasterizer_create_info.depthBiasClamp = 0.0f; // Optional
            rasterizer_create_info.depthBiasSlopeFactor = 0.0f; // Optional

            VkPipelineMultisampleStateCreateInfo multisampling_create_info{};
            multisampling_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            multisampling_create_info.sampleShadingEnable = VK_FALSE;
            multisampling_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
            multisampling_create_info.minSampleShading = 1.0f; // Optional
            multisampling_create_info.pSampleMask = nullptr; // Optional
            multisampling_create_info.alphaToCoverageEnable = VK_FALSE; // Optional
            multisampling_create_info.alphaToOneEnable = VK_FALSE; // Optional

            VkPipelineColorBlendAttachmentState color_blend_attachment_create_info{};
            color_blend_attachment_create_info.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            color_blend_attachment_create_info.blendEnable = m_props.blend_mode != BM_NONE;

            if (m_props.blend_mode != BM_NONE)
            {
                switch (m_props.blend_mode)
                {
                    case BM_SRC_ALPHA:
                    {
                        color_blend_attachment_create_info.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
                        color_blend_attachment_create_info.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
                        color_blend_attachment_create_info.colorBlendOp = VK_BLEND_OP_ADD;
                        color_blend_attachment_create_info.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
                        color_blend_attachment_create_info.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
                        color_blend_attachment_create_info.alphaBlendOp = VK_BLEND_OP_ADD;
                        break;
                    }

                    case BM_ADDITIVE:
                    {
                        color_blend_attachment_create_info.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
                        color_blend_attachment_create_info.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
                        color_blend_attachment_create_info.colorBlendOp = VK_BLEND_OP_ADD;
                        color_blend_attachment_create_info.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
                        color_blend_attachment_create_info.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
                        color_blend_attachment_create_info.alphaBlendOp = VK_BLEND_OP_ADD;
                        break;
                    }
                }
            }

            VkPipelineColorBlendStateCreateInfo color_blend_create_info{};
            color_blend_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            color_blend_create_info.logicOpEnable = VK_FALSE;
            color_blend_create_info.logicOp = VK_LOGIC_OP_COPY; // Optional
            color_blend_create_info.attachmentCount = 1;
            color_blend_create_info.pAttachments = &color_blend_attachment_create_info;
            color_blend_create_info.blendConstants[0] = 0.0f; // Optional
            color_blend_create_info.blendConstants[1] = 0.0f; // Optional
            color_blend_create_info.blendConstants[2] = 0.0f; // Optional
            color_blend_create_info.blendConstants[3] = 0.0f; // Optional

            /*std::vector<VkDynamicState> dynamic_states =
            {
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_LINE_WIDTH
            };

            VkPipelineDynamicStateCreateInfo dynamic_state_create_info{};
            dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            dynamic_state_create_info.dynamicStateCount = dynamic_states.size();
            dynamic_state_create_info.pDynamicStates = dynamic_states.data();*/

            VkPipelineLayoutCreateInfo layout_create_info{};
            layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            layout_create_info.setLayoutCount = 0; // Optional
            layout_create_info.pSetLayouts = nullptr; // Optional
            layout_create_info.pushConstantRangeCount = 0; // Optional
            layout_create_info.pPushConstantRanges = nullptr; // Optional

            if (vkCreatePipelineLayout(m_data->device, &layout_create_info, nullptr, &m_layout) != VK_SUCCESS)
                throw std::runtime_error("Error creating Vulkan renderer: vkCreatePipelineLayout failed");
            
            // Create pipeline
            VkGraphicsPipelineCreateInfo pipeline_create_info{};
            pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

            std::shared_ptr<VulkanShaderProgram> vk_shader = std::dynamic_pointer_cast<VulkanShaderProgram>(m_props.shader);
            pipeline_create_info.stageCount = vk_shader->GetPipelineInfos().size();
            pipeline_create_info.pStages = vk_shader->GetPipelineInfos().data();

            pipeline_create_info.pVertexInputState = &vertex_input_create_info;
            pipeline_create_info.pInputAssemblyState = &input_assembly_create_info;
            pipeline_create_info.pViewportState = &viewport_create_info;
            pipeline_create_info.pRasterizationState = &rasterizer_create_info;
            pipeline_create_info.pMultisampleState = &multisampling_create_info;
            pipeline_create_info.pDepthStencilState = nullptr; // Optional
            pipeline_create_info.pColorBlendState = &color_blend_create_info;
            pipeline_create_info.pDynamicState = nullptr; // Optional

            pipeline_create_info.layout = m_layout;
            pipeline_create_info.renderPass = m_data->render_pass;
            pipeline_create_info.subpass = 0;
            pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE; // Optional
            pipeline_create_info.basePipelineIndex = -1; // Optional

            if (vkCreateGraphicsPipelines(m_data->device, VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &m_pipeline) != VK_SUCCESS)
                throw std::runtime_error("Error creating Vulkan context: vkCreateGraphicsPipelines failed");
        }

        VulkanRenderer::~VulkanRenderer()
        {
            vkDeviceWaitIdle(m_data->device);
            
            if (m_have_pipline)
            {
                vkDestroyPipeline(m_data->device, m_pipeline, nullptr);
                vkDestroyPipelineLayout(m_data->device, m_layout, nullptr);
            }
        }
    
        const RendererProperties &VulkanRenderer::GetProperties() const { return m_props; }

        void VulkanRenderer::Begin()
        {
            if (m_cached_extent.width != m_data->swapchain_extent.width || m_cached_extent.height != m_data->swapchain_extent.height)
            {
                m_cached_extent = m_data->swapchain_extent;
                SetupPipeline();
            }

            VkCommandBufferBeginInfo command_buffer_begin_info{};
            command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            command_buffer_begin_info.flags = 0; // Optional
            command_buffer_begin_info.pInheritanceInfo = nullptr; // Optional

            if (vkBeginCommandBuffer(m_data->cmd_buffers[m_data->current_frame], &command_buffer_begin_info) != VK_SUCCESS)
                throw std::runtime_error("Error beginning Vulkan renderer: vkBeginCommandBuffer failed");
            
            VkRenderPassBeginInfo render_pass_begin_info{};
            render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            render_pass_begin_info.renderPass = m_data->render_pass;
            render_pass_begin_info.framebuffer = m_data->swapchain_framebuffers[m_data->current_image_index];
            render_pass_begin_info.renderArea.offset = {0, 0};
            render_pass_begin_info.renderArea.extent = m_data->swapchain_extent;
            
            VkClearValue clearColor = {{{ 0.0f, 0.0f, 0.0f, 1.0f }}};
            render_pass_begin_info.clearValueCount = 1;
            render_pass_begin_info.pClearValues = &clearColor;

            vkCmdBeginRenderPass(m_data->cmd_buffers[m_data->current_frame], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
            vkCmdBindPipeline(m_data->cmd_buffers[m_data->current_frame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
        }

        void VulkanRenderer::Render(std::shared_ptr<Mesh> mesh)
        {
            VulkanMeshData *mesh_data = reinterpret_cast<VulkanMeshData*>(mesh->GetHandle());
            VkDeviceSize offset = { 0 };
            vkCmdBindVertexBuffers(m_data->cmd_buffers[m_data->current_frame], 0, 1, &mesh_data->buffers[0], &offset);

            uint32_t num_vertices = mesh_data->num_vertices;
            if (mesh_data->num_indices > 0)
            {
                vkCmdBindIndexBuffer(m_data->cmd_buffers[m_data->current_frame], mesh_data->buffers[1], offset, VK_INDEX_TYPE_UINT32);
                num_vertices = mesh_data->num_indices;
            }
            
            vkCmdDraw(m_data->cmd_buffers[m_data->current_frame],num_vertices, 1, 0, 0);
        }

        void VulkanRenderer::End()
        {
            vkCmdEndRenderPass(m_data->cmd_buffers[m_data->current_frame]);
            if (vkEndCommandBuffer(m_data->cmd_buffers[m_data->current_frame]) != VK_SUCCESS)
                throw std::runtime_error("Error ending Vulkan renderer: vkEndCommandBuffer failed");
        }
    }
}

#endif