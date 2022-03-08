#include"OpenGLRenderer.h"

#ifdef RUT_HAS_OPENGL

#include"OpenGLMesh.h"
#include"OpenGLShader.h"
#include"OpenGLUtils.h"

#include<cassert>

namespace rut
{
    namespace impl
    {
        OpenGLRenderer::OpenGLRenderer(Context *context, const RendererProperties &props):
            m_props(props)
        {}

        OpenGLRenderer::~OpenGLRenderer()
        {}

        const RendererProperties &OpenGLRenderer::GetProperties() const { return m_props; }

        void OpenGLRenderer::Begin()
        {
            // Culling
            if (m_props.cull_mode == CM_NONE)
                glDisable(GL_CULL_FACE);
            else
            {
                glEnable(GL_CULL_FACE);
                glFrontFace(GL_CCW);

                switch (m_props.cull_mode)
                {
                    case CM_CLOCKWISE:
                        glCullFace(GL_BACK);
                        break;
                    
                    case CM_COUNTER_CLOCKWISE:
                        glCullFace(GL_FRONT);
                        break;
                    
                    case CM_BOTH:
                        glCullFace(GL_FRONT_AND_BACK);
                        break;
                }
            }

            // Blending
            if (m_props.blend_mode == BM_NONE)
                glDisable(GL_BLEND);
            else
            {
                glEnable(GL_BLEND);

                switch (m_props.blend_mode)
                {
                    case BM_SRC_ALPHA:
                        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                        break;
                    
                    case BM_ADDITIVE:
                        glBlendFunc(GL_ONE, GL_ONE);
                        break;
                }
            }

            // Depth testing
            if (m_props.depth_props.mode == DM_NONE)
                glDisable(GL_DEPTH_TEST);
            else
            {
                glDepthMask(m_props.depth_props.enable_write);

                switch (m_props.depth_props.mode)
                {
                    case DM_LESS:
                        glDepthFunc(GL_LESS);
                        break;
                    
                    case DM_GREATER:
                        glDepthFunc(GL_GREATER);
                        break;
                }
            }

            glUseProgram(static_cast<GLuint>(m_props.shader->GetHandle()));
        }

        void OpenGLRenderer::Render(std::shared_ptr<Mesh> mesh)
        {
            std::shared_ptr<OpenGLMesh> gl_mesh = std::dynamic_pointer_cast<OpenGLMesh>(mesh);
            assert(gl_mesh != nullptr);

            GLuint vao = static_cast<GLuint>(gl_mesh->GetHandle());
            glBindVertexArray(vao);

            if (gl_mesh->GetNumIndices() == 0)
                glDrawArrays(GL_TRIANGLES, 0, gl_mesh->GetNumVertices());
            else
                glDrawElements(GL_TRIANGLES, gl_mesh->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
        }

        void OpenGLRenderer::End()
        {}
    }
}

#endif