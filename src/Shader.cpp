#include"RUT/Shader.h"
#include"RUT/Config.h"

#ifdef RUT_HAS_OPENGL
#include"impl/OpenGL/OpenGLShader.h"

std::shared_ptr<rut::ShaderUnit> rut::ShaderUnit::Create(Context *context, ShaderType type, const std::string &source)
{
    return std::make_shared<rut::impl::OpenGLShaderUnit>(context, type, source);
}

std::shared_ptr<rut::ShaderProgram> rut::ShaderProgram::Create(Context *context, const ShaderProgramProperties &props)
{
    return std::make_shared<rut::impl::OpenGLShaderProgram>(context, props);
}

#endif