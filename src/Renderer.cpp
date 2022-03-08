#include"RUT/Renderer.h"
#include"RUT/Config.h"

#ifdef RUT_HAS_OPENGL
#include"impl/OpenGL/OpenGLRenderer.h"

std::shared_ptr<rut::Renderer> rut::Renderer::Create(Context *context, const RendererProperties &props)
{
    return std::make_shared<rut::impl::OpenGLRenderer>(context, props);
}

#endif