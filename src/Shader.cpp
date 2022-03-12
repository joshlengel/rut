#include"RUT/Shader.h"
#include"RUT/Config.h"
#include"RUT/Api.h"

#include<stdexcept>

#ifdef RUT_HAS_OPENGL
#include"impl/OpenGL/OpenGLShader.h"
#endif

#ifdef RUT_HAS_VULKAN
#include"impl/Vulkan/VulkanShader.h"
#endif

std::shared_ptr<rut::ShaderUnit> rut::ShaderUnit::Create(Context *context, ShaderType type, const std::string &source)
{
    switch (Api::GetRenderApi())
    {
    case RENDER_API_NONE:
        throw std::runtime_error("Error creating shader unit. RENDER_API_NONE selected");

    default:
        throw std::runtime_error("Error creating shader unit. Invalid api selected");
    
#ifdef RUT_HAS_OPENGL
    case RENDER_API_OPENGL:
        return std::make_shared<rut::impl::OpenGLShaderUnit>(context, type, source);
#endif
#ifdef RUT_HAS_VULKAN
    case RENDER_API_VULKAN:
        return std::make_shared<rut::impl::VulkanShaderUnit>(context, type, source);
#endif
    }
}

std::shared_ptr<rut::ShaderProgram> rut::ShaderProgram::Create(Context *context, const ShaderProgramCreateProperties &create_props)
{
    switch (Api::GetRenderApi())
    {
    case RENDER_API_NONE:
        throw std::runtime_error("Error creating shader program. RENDER_API_NONE selected");

    default:
        throw std::runtime_error("Error creating shader program. Invalid api selected");

#ifdef RUT_HAS_OPENGL
    case RENDER_API_OPENGL:
        return std::make_shared<rut::impl::OpenGLShaderProgram>(context, create_props);
#endif
#ifdef RUT_HAS_VULKAN
    case RENDER_API_VULKAN:
        return std::make_shared<rut::impl::VulkanShaderProgram>(context, create_props);
#endif
    }
}