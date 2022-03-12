#include"OpenGLUniformBuffer.h"

#ifdef RUT_HAS_OPENGL

#include<cstring>

namespace rut
{
    namespace impl
    {
        OpenGLUniformBuffer::OpenGLUniformBuffer(Context *context, const VertexLayout &layout):
            m_layout(layout)
        { Init(); }

        OpenGLUniformBuffer::OpenGLUniformBuffer(Context *context, VertexLayout &&layout):
            m_layout(std::move(layout))
        { Init(); }

        void OpenGLUniformBuffer::Init()
        {
            glGenBuffers(1, &m_id);
            glBindBuffer(GL_UNIFORM_BUFFER, m_id);
            glBufferData(GL_UNIFORM_BUFFER, m_layout.GetStride(), nullptr, GL_DYNAMIC_DRAW);

            m_data.resize(m_layout.GetStride());
        }

        OpenGLUniformBuffer::~OpenGLUniformBuffer()
        {
            glDeleteBuffers(1, &m_id);
        }

        const VertexLayout &OpenGLUniformBuffer::GetLayout() const { return m_layout; }

        void OpenGLUniformBuffer::Map() {}
        void OpenGLUniformBuffer::Unmap()
        {
            glBindBuffer(GL_UNIFORM_BUFFER, m_id);
            glBufferData(GL_UNIFORM_BUFFER, m_data.size(), m_data.data(), GL_DYNAMIC_DRAW);
        }

#define SET_UNIFORM(name, num_elements, value)\
auto itr = std::find_if(m_layout.begin(), m_layout.end(), [&](const VertexLayoutItem &item){ return item.name == name; });\
if (itr == m_layout.end()) return;\
std::memcpy(&m_data[itr->offset], value, num_elements * sizeof(*value));

        void OpenGLUniformBuffer::SetVariable(const std::string &name, int i)               { SET_UNIFORM(name, 1, &i) }
        void OpenGLUniformBuffer::SetVariable(const std::string &name, float f)             { SET_UNIFORM(name, 1, &f) }
        void OpenGLUniformBuffer::SetVariable(const std::string &name, const glm::ivec2 &v) { SET_UNIFORM(name, 1, &v) }
        void OpenGLUniformBuffer::SetVariable(const std::string &name, const glm::vec2 &v)  { SET_UNIFORM(name, 1, &v) }
        void OpenGLUniformBuffer::SetVariable(const std::string &name, const glm::ivec3 &v) { SET_UNIFORM(name, 1, &v) }
        void OpenGLUniformBuffer::SetVariable(const std::string &name, const glm::vec3 &v)  { SET_UNIFORM(name, 1, &v) }
        void OpenGLUniformBuffer::SetVariable(const std::string &name, const glm::ivec4 &v) { SET_UNIFORM(name, 1, &v) }
        void OpenGLUniformBuffer::SetVariable(const std::string &name, const glm::vec4 &v)  { SET_UNIFORM(name, 1, &v) }
        void OpenGLUniformBuffer::SetVariable(const std::string &name, const glm::mat3 &m)  { SET_UNIFORM(name, 1, &m) }
        void OpenGLUniformBuffer::SetVariable(const std::string &name, const glm::mat4 &m)  { SET_UNIFORM(name, 1, &m) }
        void OpenGLUniformBuffer::SetVariable(const std::string &name, uint32_t num_elements, const int *i)        { SET_UNIFORM(name, num_elements, i); }
        void OpenGLUniformBuffer::SetVariable(const std::string &name, uint32_t num_elements, const float *f)      { SET_UNIFORM(name, num_elements, f); }
        void OpenGLUniformBuffer::SetVariable(const std::string &name, uint32_t num_elements, const glm::ivec2 *v) { SET_UNIFORM(name, num_elements, v); }
        void OpenGLUniformBuffer::SetVariable(const std::string &name, uint32_t num_elements, const glm::vec2 *v)  { SET_UNIFORM(name, num_elements, v); }
        void OpenGLUniformBuffer::SetVariable(const std::string &name, uint32_t num_elements, const glm::ivec3 *v) { SET_UNIFORM(name, num_elements, v); }
        void OpenGLUniformBuffer::SetVariable(const std::string &name, uint32_t num_elements, const glm::vec3 *v)  { SET_UNIFORM(name, num_elements, v); }
        void OpenGLUniformBuffer::SetVariable(const std::string &name, uint32_t num_elements, const glm::ivec4 *v) { SET_UNIFORM(name, num_elements, v); }
        void OpenGLUniformBuffer::SetVariable(const std::string &name, uint32_t num_elements, const glm::vec4 *v)  { SET_UNIFORM(name, num_elements, v); }
        void OpenGLUniformBuffer::SetVariable(const std::string &name, uint32_t num_elements, const glm::mat3 *m)  { SET_UNIFORM(name, num_elements, m); }
        void OpenGLUniformBuffer::SetVariable(const std::string &name, uint32_t num_elements, const glm::mat4 *m)  { SET_UNIFORM(name, num_elements, m); }

        uint64_t OpenGLUniformBuffer::GetHandle() const { return static_cast<uint64_t>(m_id); }
    }
}

#endif