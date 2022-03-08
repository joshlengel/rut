#pragma once

#include"RUT/Config.h"

#ifdef RUT_HAS_OPENGL
#include"RUT/Renderer.h"

namespace rut
{
    namespace impl
    {
        class OpenGLRenderer : public Renderer
        {
        public:
            OpenGLRenderer(Context *context, const RendererProperties &props);
            virtual ~OpenGLRenderer();

            virtual const RendererProperties &GetProperties() const override;

            virtual void Begin() override;
            virtual void Render(std::shared_ptr<Mesh> mesh) override;
            virtual void End() override;
        
        private:
            RendererProperties m_props;
        };
    }
}

#endif