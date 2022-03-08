#include"OpenGLShader.h"

#ifdef RUT_HAS_OPENGL

#include<stdexcept>
#include<string_view>

static const GLenum SHADER_TYPE_GLENUM[] =
{
    GL_VERTEX_SHADER,
    GL_FRAGMENT_SHADER,
    GL_GEOMETRY_SHADER
};

#define GET_UNIFORM_LOCATION(name)\
GLint location;\
auto itr = m_uniforms.find(name);\
if (itr == m_uniforms.end()) return;\
else location = itr->second;

namespace rut
{
    namespace impl
    {
        OpenGLShaderUnit::OpenGLShaderUnit(Context *context, ShaderType type, const std::string &source)
        {
            m_id = glCreateShader(SHADER_TYPE_GLENUM[type]);

            const char *c_str = source.c_str();
            glShaderSource(m_id, 1, &c_str, nullptr);
            glCompileShader(m_id);

            GLint status;
            glGetShaderiv(m_id, GL_COMPILE_STATUS, &status);
            if (!status)
            {
                GLint length;
                glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &length);
                std::string msg(length, '\0');
                glGetShaderInfoLog(m_id, length, nullptr, &msg[0]);

                throw std::runtime_error("Error compiling shader unit: " + msg);
            }
        }

        OpenGLShaderUnit::~OpenGLShaderUnit()
        {
            glDeleteShader(m_id);
        }

        uint64_t OpenGLShaderUnit::GetHandle() const { return m_id; }
    
        OpenGLShaderProgram::OpenGLShaderProgram(Context *context, const ShaderProgramProperties &props):
            m_props(props)
        {
            m_id = glCreateProgram();

            std::vector<GLuint> shaders;

            if (m_props.vertex_shader)
                shaders.push_back(static_cast<GLuint>(m_props.vertex_shader->GetHandle()));
                
            if (m_props.fragment_shader)
                shaders.push_back(static_cast<GLuint>(m_props.fragment_shader->GetHandle()));
            
            if (m_props.geometry_shader)
                shaders.push_back(static_cast<GLuint>(m_props.geometry_shader->GetHandle()));
            
            for (GLuint shader : shaders)
                glAttachShader(m_id, shader);
            
            size_t index = 0;
            for (const auto &element : m_props.layout)
                glBindAttribLocation(m_id, index++, element.name.c_str());
            
            // Link program
            glLinkProgram(m_id);

            GLint status;
            glGetProgramiv(m_id, GL_LINK_STATUS, &status);
            if (!status)
            {
                GLint length;
                glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &length);
                std::string msg(length, '\0');
                glGetProgramInfoLog(m_id, length, nullptr, &msg[0]);

                throw std::runtime_error("Error linking shader program: " + msg);
            }

            // Get all uniforms
            GLint num_uniforms;
            glGetProgramiv(m_id, GL_ACTIVE_UNIFORMS, &num_uniforms);
            m_uniforms.reserve(num_uniforms);

            for (size_t i = 0; i < num_uniforms; ++i)
            {
                std::array<char, 255> name;
                GLsizei length;
                glGetActiveUniformName(m_id, i, name.size(), &length, &name[0]);

                // Compatibility
                std::string_view name_sv(name.data(), length);
                if (name_sv.substr(length - 3) == "[0]")
                    name_sv = name_sv.substr(0, length - 3);
                
                std::string name_s(name_sv);
                GLint location = glGetUniformLocation(m_id, name_s.c_str());
                m_uniforms[std::move(name_s)] = location;
            }

            for (GLuint shader : shaders)
                glDetachShader(m_id, shader);
        }

        OpenGLShaderProgram::~OpenGLShaderProgram()
        {
            glDeleteProgram(m_id);
        }

        const ShaderProgramProperties &OpenGLShaderProgram::GetProperties() const { return m_props; }

        void OpenGLShaderProgram::Map() { glUseProgram(m_id); }
        void OpenGLShaderProgram::Unmap() {}

        void OpenGLShaderProgram::SetVariable(const std::string &name, int i)                { GET_UNIFORM_LOCATION(name) glUniform1i(location, i); }
        void OpenGLShaderProgram::SetVariable(const std::string &name, float f)              { GET_UNIFORM_LOCATION(name) glUniform1f(location, f); }
        void OpenGLShaderProgram::SetVariable(const std::string &name, const glm::ivec2 &v)  { GET_UNIFORM_LOCATION(name) glUniform2i(location, v.x, v.y); }
        void OpenGLShaderProgram::SetVariable(const std::string &name, const glm::vec2 &v)   { GET_UNIFORM_LOCATION(name) glUniform2f(location, v.x, v.y); }
        void OpenGLShaderProgram::SetVariable(const std::string &name, const glm::ivec3 &v)  { GET_UNIFORM_LOCATION(name) glUniform3i(location, v.x, v.y, v.z); }
        void OpenGLShaderProgram::SetVariable(const std::string &name, const glm::vec3 &v)   { GET_UNIFORM_LOCATION(name) glUniform3f(location, v.x, v.y, v.z); }
        void OpenGLShaderProgram::SetVariable(const std::string &name, const glm::ivec4 &v)  { GET_UNIFORM_LOCATION(name) glUniform4i(location, v.x, v.y, v.z, v.w); }
        void OpenGLShaderProgram::SetVariable(const std::string &name, const glm::vec4 &v)   { GET_UNIFORM_LOCATION(name) glUniform4f(location, v.x, v.y, v.z, v.w); }
        void OpenGLShaderProgram::SetVariable(const std::string &name, const glm::mat3 &m)   { GET_UNIFORM_LOCATION(name) glUniformMatrix3fv(location, 1, GL_FALSE, &m[0][0]); }
        void OpenGLShaderProgram::SetVariable(const std::string &name, const glm::mat4 &m)   { GET_UNIFORM_LOCATION(name) glUniformMatrix4fv(location, 1, GL_FALSE, &m[0][0]); }

        void OpenGLShaderProgram::SetVariableArray(const std::string &name, size_t num_elements, const int *i)        { GET_UNIFORM_LOCATION(name) glUniform1iv(location, num_elements, i); }
        void OpenGLShaderProgram::SetVariableArray(const std::string &name, size_t num_elements, const float *f)      { GET_UNIFORM_LOCATION(name) glUniform1fv(location, num_elements, f); }
        void OpenGLShaderProgram::SetVariableArray(const std::string &name, size_t num_elements, const glm::ivec2 *v) { GET_UNIFORM_LOCATION(name) glUniform2iv(location, num_elements, reinterpret_cast<const GLint*>(v)); }
        void OpenGLShaderProgram::SetVariableArray(const std::string &name, size_t num_elements, const glm::vec2 *v)  { GET_UNIFORM_LOCATION(name) glUniform2fv(location, num_elements, reinterpret_cast<const GLfloat*>(v)); }
        void OpenGLShaderProgram::SetVariableArray(const std::string &name, size_t num_elements, const glm::ivec3 *v) { GET_UNIFORM_LOCATION(name) glUniform3iv(location, num_elements, reinterpret_cast<const GLint*>(v)); }
        void OpenGLShaderProgram::SetVariableArray(const std::string &name, size_t num_elements, const glm::vec3 *v)  { GET_UNIFORM_LOCATION(name) glUniform3fv(location, num_elements, reinterpret_cast<const GLfloat*>(v)); }
        void OpenGLShaderProgram::SetVariableArray(const std::string &name, size_t num_elements, const glm::ivec4 *v) { GET_UNIFORM_LOCATION(name) glUniform4iv(location, num_elements, reinterpret_cast<const GLint*>(v)); }
        void OpenGLShaderProgram::SetVariableArray(const std::string &name, size_t num_elements, const glm::vec4 *v)  { GET_UNIFORM_LOCATION(name) glUniform4fv(location, num_elements, reinterpret_cast<const GLfloat*>(v)); }
        void OpenGLShaderProgram::SetVariableArray(const std::string &name, size_t num_elements, const glm::mat3 *m)  { GET_UNIFORM_LOCATION(name) glUniformMatrix3fv(location, num_elements, GL_FALSE, reinterpret_cast<const GLfloat*>(m)); }
        void OpenGLShaderProgram::SetVariableArray(const std::string &name, size_t num_elements, const glm::mat4 *m)  { GET_UNIFORM_LOCATION(name) glUniformMatrix4fv(location, num_elements, GL_FALSE, reinterpret_cast<const GLfloat*>(m)); }

        uint64_t OpenGLShaderProgram::GetHandle() const { return m_id; }
    }
}

#endif