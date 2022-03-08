#include"RUT/Renderer.h"
#include"RUT/Config.h"
#include"RUT/Api.h"

#include<stdexcept>

#ifdef RUT_HAS_OPENGL
#include"impl/OpenGL/OpenGLRenderer.h"
#endif

std::shared_ptr<rut::Renderer> rut::Renderer::Create(Context *context, const RendererProperties &props)
{
    switch (Api::GetRenderApi())
    {
    case RENDER_API_NONE:
        throw std::runtime_error("Error creating renderer. RENDER_API_NONE selected");

#ifdef RUT_HAS_OPENGL
    case RENDER_API_OPENGL:
        return std::make_shared<rut::impl::OpenGLRenderer>(context, props);
#endif
    }
}