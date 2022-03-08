#pragma once

#include"Mesh.h"

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
    enum ShaderType
    {
        ST_VERTEX,
        ST_FRAGMENT,
        ST_GEOMETRY
    };

    class ShaderUnit
    {
    public:
        virtual ~ShaderUnit() = default;

        virtual uint64_t GetHandle() const = 0;

        static std::shared_ptr<ShaderUnit> Create(Context *context, ShaderType type, const std::string &source);
    };

    struct ShaderProgramProperties
    {
        std::shared_ptr<ShaderUnit> vertex_shader;
        std::shared_ptr<ShaderUnit> fragment_shader;
        std::shared_ptr<ShaderUnit> geometry_shader;

        VertexLayout layout = {};
    };

    class ShaderProgram
    {
    public:
        virtual ~ShaderProgram() = default;

        virtual const ShaderProgramProperties &GetProperties() const = 0;

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

        virtual void SetVariableArray(const std::string &name, size_t num_elements, const int *i) = 0;
        virtual void SetVariableArray(const std::string &name, size_t num_elements, const float *f) = 0;
        virtual void SetVariableArray(const std::string &name, size_t num_elements, const glm::ivec2 *v) = 0;
        virtual void SetVariableArray(const std::string &name, size_t num_elements, const glm::vec2 *v) = 0;
        virtual void SetVariableArray(const std::string &name, size_t num_elements, const glm::ivec3 *v) = 0;
        virtual void SetVariableArray(const std::string &name, size_t num_elements, const glm::vec3 *v) = 0;
        virtual void SetVariableArray(const std::string &name, size_t num_elements, const glm::ivec4 *v) = 0;
        virtual void SetVariableArray(const std::string &name, size_t num_elements, const glm::vec4 *v) = 0;
        virtual void SetVariableArray(const std::string &name, size_t num_elements, const glm::mat3 *m) = 0;
        virtual void SetVariableArray(const std::string &name, size_t num_elements, const glm::mat4 *m) = 0;

        virtual uint64_t GetHandle() const = 0;

        static std::shared_ptr<ShaderProgram> Create(Context *context, const ShaderProgramProperties &props);
    };
}