#pragma once

#include"Layout.h"

#include<cstdint>
#include<string>
#include<memory>

namespace rut
{
    class Context;
    class UniformBuffer;

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

        virtual ShaderType GetType() const = 0;

        virtual uint64_t GetHandle() const = 0;

        static std::shared_ptr<ShaderUnit> Create(Context *context, ShaderType type, const std::string &source);
    };

    struct UniformBindingProperties
    {
        uint32_t binding;
        std::string name;
        ShaderType stage;
        VertexLayout layout = {};
    };

    struct ShaderProgramProperties
    {
        VertexLayout input_layout = {};
        std::vector<UniformBindingProperties> uniform_bindings;
    };

    struct ShaderProgramCreateProperties
    {
        std::shared_ptr<ShaderUnit> vertex_shader;
        std::shared_ptr<ShaderUnit> fragment_shader;
        std::shared_ptr<ShaderUnit> geometry_shader;

        ShaderProgramProperties props = {};
    };

    class ShaderProgram
    {
    public:
        virtual ~ShaderProgram() = default;

        virtual const ShaderProgramProperties &GetProperties() const = 0;

        virtual void BindUniformBuffer(uint32_t binding, std::shared_ptr<UniformBuffer> buffer) = 0;

        virtual uint64_t GetHandle() const = 0;

        static std::shared_ptr<ShaderProgram> Create(Context *context, const ShaderProgramCreateProperties &create_props);
    };
}