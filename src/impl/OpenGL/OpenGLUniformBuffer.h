#pragma once

#include"RUT/Config.h"

#ifdef RUT_HAS_OPENGL

#include"RUT/UniformBuffer.h"
#include"RUT/Layout.h"
#include"OpenGLUtils.h"

#include<vector>

namespace rut
{
    namespace impl
    {
        class OpenGLUniformBuffer : public UniformBuffer
        {
        public:
            OpenGLUniformBuffer(Context *context, const VertexLayout &layout);
            OpenGLUniformBuffer(Context *context, VertexLayout &&layout);
            virtual ~OpenGLUniformBuffer();

            virtual const VertexLayout &GetLayout() const override;

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
            virtual void SetVariable(const std::string &name, uint32_t num_elements, const int *i) override;
            virtual void SetVariable(const std::string &name, uint32_t num_elements, const float *f) override;
            virtual void SetVariable(const std::string &name, uint32_t num_elements, const glm::ivec2 *v) override;
            virtual void SetVariable(const std::string &name, uint32_t num_elements, const glm::vec2 *v) override;
            virtual void SetVariable(const std::string &name, uint32_t num_elements, const glm::ivec3 *v) override;
            virtual void SetVariable(const std::string &name, uint32_t num_elements, const glm::vec3 *v) override;
            virtual void SetVariable(const std::string &name, uint32_t num_elements, const glm::ivec4 *v) override;
            virtual void SetVariable(const std::string &name, uint32_t num_elements, const glm::vec4 *v) override;
            virtual void SetVariable(const std::string &name, uint32_t num_elements, const glm::mat3 *m) override;
            virtual void SetVariable(const std::string &name, uint32_t num_elements, const glm::mat4 *m) override;

            virtual uint64_t GetHandle() const override;
        
        private:
            VertexLayout m_layout;
            GLuint m_id;

            std::vector<char> m_data;

            void Init();
        };
    }
}

#endif