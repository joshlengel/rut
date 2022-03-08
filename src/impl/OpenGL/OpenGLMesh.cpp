#include"OpenGLMesh.h"

#ifdef RUT_HAS_OPENGL

static const bool VERTEX_TYPE_IS_FLOAT[] =
{
    false, true, false, true, false, true, false, true, true, true
};

static GLenum VERTEX_TYPE_GLENUM[] =
{
    GL_INT, GL_FLOAT, GL_INT, GL_FLOAT, GL_INT, GL_FLOAT, GL_INT, GL_FLOAT, GL_FLOAT, GL_FLOAT,
};

namespace rut
{
    namespace impl
    {
        OpenGLMesh::OpenGLMesh(rut::Context *context, const rut::VertexLayout &layout):
            m_layout(layout)
        { Init(); }

        OpenGLMesh::OpenGLMesh(rut::Context *context, rut::VertexLayout &&layout):
            m_layout(std::move(layout))
        { Init(); }

        void OpenGLMesh::Init()
        {
            glGenVertexArrays(1, &m_vao);
            glGenBuffers(2, m_buffers);

            glBindVertexArray(m_vao);

            glBindBuffer(GL_ARRAY_BUFFER, m_buffers[0]);

            size_t index = 0;
            for (const auto &item : m_layout)
            {
                if (VERTEX_TYPE_IS_FLOAT[item.type])
                    glVertexAttribPointer(index, item.count, VERTEX_TYPE_GLENUM[item.type], GL_FALSE, m_layout.GetStride(), reinterpret_cast<void*>(item.offset));
                else
                    glVertexAttribIPointer(index, item.count, VERTEX_TYPE_GLENUM[item.type], m_layout.GetStride(), reinterpret_cast<void*>(item.offset));
                
                glEnableVertexAttribArray(index);
                ++index;
            }
        }

        OpenGLMesh::~OpenGLMesh()
        {
            glDeleteVertexArrays(1, &m_vao);
            glDeleteBuffers(2, m_buffers);
        }

        const VertexLayout &OpenGLMesh::GetLayout() const { return m_layout; }

        void OpenGLMesh::SetVertices(size_t num_vertices, const void *vertices)
        {
            m_num_vertices = num_vertices;

            glBindVertexArray(m_vao);
            glBindBuffer(GL_ARRAY_BUFFER, m_buffers[0]);
            glBufferData(GL_ARRAY_BUFFER, num_vertices * m_layout.GetStride(), vertices, GL_STATIC_DRAW);
        }

        void OpenGLMesh::SetIndices(size_t num_indices, const uint32_t *indices)
        {
            m_num_indices = num_indices;

            glBindVertexArray(m_vao);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[1]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_indices * sizeof(uint32_t), indices, GL_STATIC_DRAW);
        }

        uint64_t OpenGLMesh::GetHandle() const { return m_vao; }

        size_t OpenGLMesh::GetNumVertices() const { return m_num_vertices; }
        size_t OpenGLMesh::GetNumIndices() const { return m_num_indices; }
    }
}

#endif