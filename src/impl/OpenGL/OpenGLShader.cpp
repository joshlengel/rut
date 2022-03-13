#include"OpenGLShader.h"

#ifdef RUT_HAS_OPENGL

#include"OpenGLUniformBuffer.h"
#include"ShaderTools.h"

#include<stdexcept>
#include<string_view>

static const GLenum SHADER_TYPE_GLENUM[] =
{
    GL_VERTEX_SHADER,
    GL_FRAGMENT_SHADER,
    GL_GEOMETRY_SHADER
};

static const uint32_t VERTEX_TYPE_BYTES[] =
{
    4, 4, 8, 8, 12, 12, 16, 16
};

static const uint32_t VERTEX_TYPE_COUNTS[] =
{
    1, 1, 2, 2, 3, 3, 4, 4, 9, 16
};

namespace rut
{
    namespace impl
    {
        OpenGLShaderUnit::OpenGLShaderUnit(Context *context, ShaderType type, const std::string &source):
            m_type(type)
        {
            m_id = glCreateShader(SHADER_TYPE_GLENUM[type]);

            CodeBlob blob = GenerateOpenGLShader(source, type);
            GLint length = blob.bytes;
            glShaderSource(m_id, 1, &blob.data, &length);
            delete[] blob.data;

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

        ShaderType OpenGLShaderUnit::GetType() const { return m_type; }

        uint64_t OpenGLShaderUnit::GetHandle() const { return m_id; }

        OpenGLShaderProgram::OpenGLShaderProgram(Context *context, const ShaderProgramCreateProperties &create_props):
            m_props(create_props.props)
        {
            m_id = glCreateProgram();

            std::vector<GLuint> shaders;
            auto ProcessShader = [&](std::shared_ptr<ShaderUnit> unit)
            {
                shaders.push_back(static_cast<GLuint>(unit->GetHandle()));
            };

            if (create_props.vertex_shader)
                ProcessShader(create_props.vertex_shader);
                
            if (create_props.fragment_shader)
                ProcessShader(create_props.fragment_shader);
            
            if (create_props.geometry_shader)
                ProcessShader(create_props.geometry_shader);
            
            for (GLuint shader : shaders)
                glAttachShader(m_id, shader);
            
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

            for (const auto &uniform_binding : m_props.uniform_bindings)
            {
                GLint index = glGetUniformBlockIndex(m_id, uniform_binding.name.c_str());
                glUniformBlockBinding(m_id, index, uniform_binding.binding);
                m_binding_point_indices[uniform_binding.binding] = index;
            }

            for (GLuint shader : shaders)
                glDetachShader(m_id, shader);
        }

        OpenGLShaderProgram::~OpenGLShaderProgram()
        {
            glDeleteProgram(m_id);
        }

        const ShaderProgramProperties &OpenGLShaderProgram::GetProperties() const { return m_props; }

        void OpenGLShaderProgram::BindUniformBuffer(uint32_t binding, std::shared_ptr<UniformBuffer> buffer)
        {
            // Update binding
            glBindBufferBase(GL_UNIFORM_BUFFER, binding, static_cast<GLuint>(buffer->GetHandle()));

            // Update layout
            GLint num_uniforms;
            glGetActiveUniformBlockiv(m_id, m_binding_point_indices[binding], GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &num_uniforms);
            std::vector<GLint> uniform_indices(num_uniforms);
            glGetActiveUniformBlockiv(m_id, m_binding_point_indices[binding], GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, &uniform_indices[0]);

            uint32_t index = 0;
            std::shared_ptr<OpenGLUniformBuffer> gl_buffer = std::dynamic_pointer_cast<OpenGLUniformBuffer>(buffer);
            for (auto &item : gl_buffer->m_layout)
            {
                GLuint uniform_index = static_cast<GLuint>(uniform_indices[index]);
                GLint offset;
                glGetActiveUniformsiv(m_id, 1, &uniform_index, GL_UNIFORM_OFFSET, &offset);
                item->SetOffset(offset);

                LayoutItem *base_item = item;
                if (base_item->GetLength() > 1)
                {
                    GLint stride;
                    glGetActiveUniformsiv(m_id, 1, &uniform_index, GL_UNIFORM_ARRAY_STRIDE, &stride);
                    UniformArrayLayoutItem *arr_item = dynamic_cast<UniformArrayLayoutItem*>(item);
                    arr_item->SetStride(stride);
                    arr_item->SetSize(stride * arr_item->GetLength());
                    base_item = arr_item->GetItem();
                }

                if (base_item->GetType() == VT_MAT3 || base_item->GetType() == VT_MAT4)
                {
                    GLint stride;
                    glGetActiveUniformsiv(m_id, 1, &uniform_index, GL_UNIFORM_MATRIX_STRIDE, &stride);
                    UniformMatrixLayoutItem *mat_item = dynamic_cast<UniformMatrixLayoutItem*>(base_item);
                    mat_item->SetStride(stride);
                    base_item->SetSize(stride * mat_item->GetDimension());
                }
                else
                {
                    base_item->SetSize(VERTEX_TYPE_BYTES[base_item->GetType()]);
                }

                ++index;
            }

            GLint stride;
            glGetActiveUniformBlockiv(m_id, m_binding_point_indices[binding], GL_UNIFORM_BLOCK_DATA_SIZE, &stride);
            gl_buffer->m_layout.SetStride(stride);

            glBindBuffer(GL_UNIFORM_BUFFER, gl_buffer->m_id);
            glBufferData(GL_UNIFORM_BUFFER, stride, nullptr, GL_DYNAMIC_DRAW);
            gl_buffer->m_data.resize(stride);
        }

        uint64_t OpenGLShaderProgram::GetHandle() const { return m_id; }
    }
}

#endif