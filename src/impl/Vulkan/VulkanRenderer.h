#pragma once

#include"RUT/Config.h"

#ifdef RUT_HAS_VULKAN

#include"RUT/Renderer.h"
#include"VulkanUtils.h"

#include<vulkan/vulkan.h>

namespace rut
{
    namespace impl
    {
        class VulkanRenderer : public Renderer
        {
        public:
            VulkanRenderer(Context *context, const RendererProperties &props);
            virtual ~VulkanRenderer();
        
            virtual const RendererProperties &GetProperties() const override;

            virtual void Begin() override;
            virtual void Render(std::shared_ptr<Mesh> mesh) override;
            virtual void End() override;
        
        private:
            VulkanData *m_data;
            RendererProperties m_props;
            VkExtent2D m_cached_extent;
            bool m_have_pipline = false;
            VkPipelineLayout m_layout;
            VkPipeline m_pipeline;

            void SetupPipeline();
        };
    }
}

#endif