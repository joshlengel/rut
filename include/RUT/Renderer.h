#pragma once

#include<memory>

#include<glm/vec4.hpp>

namespace rut
{
    class Mesh;
    class ShaderProgram;
    class Context;

    enum CullMode
    {
        CM_NONE,
        CM_CLOCKWISE,
        CM_COUNTER_CLOCKWISE,
        CM_BOTH
    };

    enum BlendMode
    {
        BM_NONE,
        BM_SRC_ALPHA,
        BM_ADDITIVE
    };

    enum DepthMode
    {
        DM_NONE,
        DM_LESS,
        DM_GREATER
    };

    struct DepthProperties
    {
        DepthMode mode = DM_NONE;
        bool enable_write = false;
    };

    struct ClearProperties
    {
        glm::vec4 clear_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        float clear_depth = 1.0f;
    };

    struct RendererProperties
    {
        CullMode cull_mode = CM_NONE;
        BlendMode blend_mode = BM_NONE;
        DepthProperties depth_props;
        ClearProperties clear_props;

        std::shared_ptr<ShaderProgram> shader;
    };

    class Renderer
    {
    public:
        virtual ~Renderer() = default;

        virtual const RendererProperties &GetProperties() const = 0;

        virtual void Begin() = 0;
        virtual void Render(std::shared_ptr<Mesh> mesh) = 0;
        virtual void End() = 0;

        static std::shared_ptr<Renderer> Create(Context *context, const RendererProperties &props);
    };
}