#include"RUT/UniformBuffer.h"
#include"RUT/Config.h"
#include"RUT/Api.h"

#include<stdexcept>

#ifdef RUT_HAS_OPENGL
#include"impl/OpenGL/OpenGLUniformBuffer.h"
#endif
#ifdef RUT_HAS_VULKAN
#include"impl/Vulkan/VulkanUniformBuffer.h"
#endif

namespace rut
{
    std::shared_ptr<rut::UniformBuffer> rut::UniformBuffer::Create(Context *context, const VertexLayout &layout)
    {
        switch (Api::GetRenderApi())
        {
        case RENDER_API_NONE:
            throw std::runtime_error("Error creating uniform buffer. RENDER_API_NONE selected");

        default:
            throw std::runtime_error("Error creating uniform buffer. Invalid api selected");

    #ifdef RUT_HAS_OPENGL
        case RENDER_API_OPENGL:
            return std::make_shared<rut::impl::OpenGLUniformBuffer>(context, layout);
    #endif
    #ifdef RUT_HAS_VULKAN
        case RENDER_API_VULKAN:
            return std::make_shared<rut::impl::VulkanUniformBuffer>(context, layout);
    #endif
        }
    }

    std::shared_ptr<rut::UniformBuffer> rut::UniformBuffer::Create(Context *context, VertexLayout &&layout)
    {
        switch (Api::GetRenderApi())
        {
        case RENDER_API_NONE:
            throw std::runtime_error("Error creating uniform buffer. RENDER_API_NONE selected");

        default:
            throw std::runtime_error("Error creating uniform buffer. Invalid api selected");

    #ifdef RUT_HAS_OPENGL
        case RENDER_API_OPENGL:
            return std::make_shared<rut::impl::OpenGLUniformBuffer>(context, std::move(layout));
    #endif
    #ifdef RUT_HAS_VULKAN
        case RENDER_API_VULKAN:
            return std::make_shared<rut::impl::VulkanUniformBuffer>(context, std::move(layout));
    #endif
        }
    }
}