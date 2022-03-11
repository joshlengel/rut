#pragma once

#include"RUT/Api.h"
#include"RUT/Shader.h"

#include<string>

namespace rut
{
    struct CodeBlob
    {
        uint32_t bytes;
        char *data;
    };

#ifdef RUT_HAS_VULKAN
    CodeBlob GenerateVulkanShader(const std::string &source, ShaderType type);
#endif
#ifdef RUT_HAS_OPENGL
    CodeBlob GenerateOpenGLShader(const std::string &source, ShaderType type);
#endif
}