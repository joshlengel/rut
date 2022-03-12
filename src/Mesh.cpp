#include"RUT/Mesh.h"
#include"RUT/Config.h"
#include"RUT/Api.h"

#include<stdexcept>

#ifdef RUT_HAS_OPENGL
#include"impl/OpenGL/OpenGLMesh.h"
#endif

#ifdef RUT_HAS_VULKAN
#include"impl/Vulkan/VulkanMesh.h"
#endif

std::shared_ptr<rut::Mesh> rut::Mesh::Create(Context *context, const rut::VertexLayout &layout)
{
    switch (Api::GetRenderApi())
    {
    case RENDER_API_NONE:
        throw std::runtime_error("Error creating mesh. RENDER_API_NONE selected");

    default:
        throw std::runtime_error("Error creating mesh. Invalid api selected");

#ifdef RUT_HAS_OPENGL
    case RENDER_API_OPENGL:
        return std::make_shared<rut::impl::OpenGLMesh>(context, layout);
#endif

#ifdef RUT_HAS_VULKAN
    case RENDER_API_VULKAN:
        return std::make_shared<rut::impl::VulkanMesh>(context, layout);
#endif
    }
}

std::shared_ptr<rut::Mesh> rut::Mesh::Create(Context *context, rut::VertexLayout &&layout)
{
    switch (Api::GetRenderApi())
    {
    case RENDER_API_NONE:
        throw std::runtime_error("Error creating mesh. RENDER_API_NONE selected");

    default:
        throw std::runtime_error("Error creating mesh. Invalid api selected");

#ifdef RUT_HAS_OPENGL
    case RENDER_API_OPENGL:
        return std::make_shared<rut::impl::OpenGLMesh>(context, std::move(layout));
#endif

#ifdef RUT_HAS_VULKAN
    case RENDER_API_VULKAN:
        return std::make_shared<rut::impl::VulkanMesh>(context, std::move(layout));
#endif
    }
}