#pragma once

#include<cstdint>
#include<string>
#include<memory>

#include<glm/vec2.hpp>
#include<glm/vec3.hpp>
#include<glm/vec4.hpp>
#include<glm/mat3x3.hpp>
#include<glm/mat4x4.hpp>

namespace rut
{
    class UniformLayout;
    class Context;

    class UniformBuffer
    {
    public:
        virtual ~UniformBuffer() = default;

        virtual const UniformLayout &GetLayout() const = 0;

        virtual void Map() = 0;
        virtual void Unmap() = 0;
        virtual void SetVariable(const std::string &name, int i) = 0;
        virtual void SetVariable(const std::string &name, float f) = 0;
        virtual void SetVariable(const std::string &name, const glm::ivec2 &v) = 0;
        virtual void SetVariable(const std::string &name, const glm::vec2 &v) = 0;
        virtual void SetVariable(const std::string &name, const glm::ivec3 &v) = 0;
        virtual void SetVariable(const std::string &name, const glm::vec3 &v) = 0;
        virtual void SetVariable(const std::string &name, const glm::ivec4 &v) = 0;
        virtual void SetVariable(const std::string &name, const glm::vec4 &v) = 0;
        virtual void SetVariable(const std::string &name, const glm::mat3 &m) = 0;
        virtual void SetVariable(const std::string &name, const glm::mat4 &m) = 0;
        virtual void SetVariable(const std::string &name, const int *i) = 0;
        virtual void SetVariable(const std::string &name, const float *f) = 0;
        virtual void SetVariable(const std::string &name, const glm::ivec2 *v) = 0;
        virtual void SetVariable(const std::string &name, const glm::vec2 *v) = 0;
        virtual void SetVariable(const std::string &name, const glm::ivec3 *v) = 0;
        virtual void SetVariable(const std::string &name, const glm::vec3 *v) = 0;
        virtual void SetVariable(const std::string &name, const glm::ivec4 *v) = 0;
        virtual void SetVariable(const std::string &name, const glm::vec4 *v) = 0;
        virtual void SetVariable(const std::string &name, const glm::mat3 *m) = 0;
        virtual void SetVariable(const std::string &name, const glm::mat4 *m) = 0;

        virtual uint64_t GetHandle() const = 0;

        static std::shared_ptr<UniformBuffer> Create(Context *context, const UniformLayout &layout);
        static std::shared_ptr<UniformBuffer> Create(Context *context, UniformLayout &&layout);
    };
}