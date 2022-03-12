#pragma once

#include"RUT/Config.h"

#ifdef RUT_HAS_OPENGL

#include"RUT/Mesh.h"
#include"RUT/Layout.h"
#include"OpenGLUtils.h"

namespace rut
{
    namespace impl
    {
        class OpenGLMesh : public Mesh
        {
        public:
            OpenGLMesh(Context *context, const VertexLayout &layout);
            OpenGLMesh(Context *context, VertexLayout &&layout);
            virtual ~OpenGLMesh();

            virtual const VertexLayout &GetLayout() const override;

            virtual void SetVertices(size_t num_vertices, const void *vertices) override;
            virtual void SetIndices(size_t num_indices, const uint32_t *indices) override;

            virtual uint64_t GetHandle() const override;

            size_t GetNumVertices() const;
            size_t GetNumIndices() const;

        private:
            VertexLayout m_layout;
            
            GLuint m_vao;
            GLuint m_buffers[2];
            size_t m_num_vertices, m_num_indices;

            void Init();
        };
    }
}
#endif