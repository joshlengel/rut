#pragma once

#include"RUT/Config.h"

#ifdef RUT_HAS_OPENGL

#include"RUT/Shader.h"
#include"OpenGLUtils.h"

#include<unordered_map>

namespace rut
{
    namespace impl
    {
        class OpenGLShaderUnit : public ShaderUnit
        {
        public:
            OpenGLShaderUnit(Context *context, ShaderType type, const std::string &source);
            virtual ~OpenGLShaderUnit();

            virtual ShaderType GetType() const override;

            virtual uint64_t GetHandle() const override;
        
        private:
            ShaderType m_type;
            GLuint m_id;
        };

        class OpenGLShaderProgram : public ShaderProgram
        {
        public:
            OpenGLShaderProgram(Context *context, const ShaderProgramCreateProperties &create_props);
            virtual ~OpenGLShaderProgram();

            virtual const ShaderProgramProperties &GetProperties() const override;

            virtual void BindUniformBuffer(uint32_t binding, std::shared_ptr<UniformBuffer> buffer) override;

            virtual uint64_t GetHandle() const override;
        
        private:
            ShaderProgramProperties m_props;
            GLuint m_id;
            std::unordered_map<GLint, GLint> m_binding_point_indices;
        };
    }
}
#endif