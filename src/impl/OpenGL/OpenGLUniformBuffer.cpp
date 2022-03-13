#include"OpenGLUniformBuffer.h"

#ifdef RUT_HAS_OPENGL

#include<cstring>

namespace rut
{
    namespace impl
    {
        OpenGLUniformBuffer::OpenGLUniformBuffer(Context *context, const UniformLayout &layout):
            m_layout(layout)
        { Init(); }

        OpenGLUniformBuffer::OpenGLUniformBuffer(Context *context, UniformLayout &&layout):
            m_layout(std::move(layout))
        { Init(); }

        void OpenGLUniformBuffer::Init()
        {
            glGenBuffers(1, &m_id);
        }

        OpenGLUniformBuffer::~OpenGLUniformBuffer()
        {
            glDeleteBuffers(1, &m_id);
        }

        const UniformLayout &OpenGLUniformBuffer::GetLayout() const { return m_layout; }

        void OpenGLUniformBuffer::Map() {}
        void OpenGLUniformBuffer::Unmap()
        {
            glBindBuffer(GL_UNIFORM_BUFFER, m_id);
            glBufferData(GL_UNIFORM_BUFFER, m_data.size(), m_data.data(), GL_DYNAMIC_DRAW);
        }

#define SET_UNIFORM(name, value)\
auto itr = std::find_if(m_layout.begin(), m_layout.end(), [&](LayoutItem *item){ return item->GetName() == name; });\
if (itr == m_layout.end()) return;\
(*itr)->Write(&m_data[(*itr)->GetOffset()], value);

        void OpenGLUniformBuffer::SetVariable(const std::string &name, int i)               { SET_UNIFORM(name, &i) }
        void OpenGLUniformBuffer::SetVariable(const std::string &name, float f)             { SET_UNIFORM(name, &f) }
        void OpenGLUniformBuffer::SetVariable(const std::string &name, const glm::ivec2 &v) { SET_UNIFORM(name, &v) }
        void OpenGLUniformBuffer::SetVariable(const std::string &name, const glm::vec2 &v)  { SET_UNIFORM(name, &v) }
        void OpenGLUniformBuffer::SetVariable(const std::string &name, const glm::ivec3 &v) { SET_UNIFORM(name, &v) }
        void OpenGLUniformBuffer::SetVariable(const std::string &name, const glm::vec3 &v)  { SET_UNIFORM(name, &v) }
        void OpenGLUniformBuffer::SetVariable(const std::string &name, const glm::ivec4 &v) { SET_UNIFORM(name, &v) }
        void OpenGLUniformBuffer::SetVariable(const std::string &name, const glm::vec4 &v)  { SET_UNIFORM(name, &v) }
        void OpenGLUniformBuffer::SetVariable(const std::string &name, const glm::mat3 &m)  { SET_UNIFORM(name, &m) }
        void OpenGLUniformBuffer::SetVariable(const std::string &name, const glm::mat4 &m)  { SET_UNIFORM(name, &m) }
        void OpenGLUniformBuffer::SetVariable(const std::string &name, const int *i)        { SET_UNIFORM(name, i); }
        void OpenGLUniformBuffer::SetVariable(const std::string &name, const float *f)      { SET_UNIFORM(name, f); }
        void OpenGLUniformBuffer::SetVariable(const std::string &name, const glm::ivec2 *v) { SET_UNIFORM(name, v); }
        void OpenGLUniformBuffer::SetVariable(const std::string &name, const glm::vec2 *v)  { SET_UNIFORM(name, v); }
        void OpenGLUniformBuffer::SetVariable(const std::string &name, const glm::ivec3 *v) { SET_UNIFORM(name, v); }
        void OpenGLUniformBuffer::SetVariable(const std::string &name, const glm::vec3 *v)  { SET_UNIFORM(name, v); }
        void OpenGLUniformBuffer::SetVariable(const std::string &name, const glm::ivec4 *v) { SET_UNIFORM(name, v); }
        void OpenGLUniformBuffer::SetVariable(const std::string &name, const glm::vec4 *v)  { SET_UNIFORM(name, v); }
        void OpenGLUniformBuffer::SetVariable(const std::string &name, const glm::mat3 *m)  { SET_UNIFORM(name, m); }
        void OpenGLUniformBuffer::SetVariable(const std::string &name, const glm::mat4 *m)  { SET_UNIFORM(name, m); }

        uint64_t OpenGLUniformBuffer::GetHandle() const { return static_cast<uint64_t>(m_id); }
    }
}

#endif