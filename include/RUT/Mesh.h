#pragma once

#include<cstdint>
#include<memory>

namespace rut
{
    class Context;
    class VertexLayout;

    class Mesh
    {
    public:
        virtual ~Mesh() = default;

        virtual const VertexLayout &GetLayout() const = 0;

        virtual void SetVertices(size_t num_vertices, const void *vertices) = 0;
        virtual void SetIndices(size_t num_indices, const uint32_t *indices) = 0;

        virtual uint64_t GetHandle() const = 0;

        static std::shared_ptr<Mesh> Create(Context *context, const VertexLayout &layout);
        static std::shared_ptr<Mesh> Create(Context *context, VertexLayout &&layout);
    };
}