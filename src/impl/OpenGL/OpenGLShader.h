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

            virtual uint64_t GetHandle() const override;
        
        private:
            GLuint m_id;
        };

        class OpenGLShaderProgram : public ShaderProgram
        {
        public:
            OpenGLShaderProgram(Context *context, const ShaderProgramProperties &props);
            virtual ~OpenGLShaderProgram();

            virtual const ShaderProgramProperties &GetProperties() const override;

            virtual void Map() override;
            virtual void Unmap() override;

            virtual void SetVariable(const std::string &name, int i) override;
            virtual void SetVariable(const std::string &name, float f) override;
            virtual void SetVariable(const std::string &name, const glm::ivec2 &v) override;
            virtual void SetVariable(const std::string &name, const glm::vec2 &v) override;
            virtual void SetVariable(const std::string &name, const glm::ivec3 &v) override;
            virtual void SetVariable(const std::string &name, const glm::vec3 &v) override;
            virtual void SetVariable(const std::string &name, const glm::ivec4 &v) override;
            virtual void SetVariable(const std::string &name, const glm::vec4 &v) override;
            virtual void SetVariable(const std::string &name, const glm::mat3 &m) override;
            virtual void SetVariable(const std::string &name, const glm::mat4 &m) override;

            virtual void SetVariableArray(const std::string &name, size_t num_elements, const int *i) override;
            virtual void SetVariableArray(const std::string &name, size_t num_elements, const float *f) override;
            virtual void SetVariableArray(const std::string &name, size_t num_elements, const glm::ivec2 *v) override;
            virtual void SetVariableArray(const std::string &name, size_t num_elements, const glm::vec2 *v) override;
            virtual void SetVariableArray(const std::string &name, size_t num_elements, const glm::ivec3 *v) override;
            virtual void SetVariableArray(const std::string &name, size_t num_elements, const glm::vec3 *v) override;
            virtual void SetVariableArray(const std::string &name, size_t num_elements, const glm::ivec4 *v) override;
            virtual void SetVariableArray(const std::string &name, size_t num_elements, const glm::vec4 *v) override;
            virtual void SetVariableArray(const std::string &name, size_t num_elements, const glm::mat3 *m) override;
            virtual void SetVariableArray(const std::string &name, size_t num_elements, const glm::mat4 *m) override;

            virtual uint64_t GetHandle() const override;
        
        private:
            ShaderProgramProperties m_props;
            GLuint m_id;

            std::unordered_map<std::string, GLint> m_uniforms;
        };
    }
}
#endif