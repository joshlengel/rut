#include"RUT/Mesh.h"
#include"RUT/Config.h"

static const uint32_t VERTEX_TYPE_BYTES[] =
{
    4, 4, 8, 8, 12, 12, 16, 16, 36, 64
};

static const uint32_t VERTEX_TYPE_COUNTS[] =
{
    1, 1, 2, 2, 3, 3, 4, 4, 9, 16
};

namespace rut
{
    VertexLayout::VertexLayout(std::initializer_list<VertexLayoutElement> il):
        m_items(il.size()),
        m_stride(0)
    {
        auto src_itr = il.begin();
        for (size_t i = 0; i < il.size(); ++i, ++src_itr)
        {
            VertexLayoutItem &item = m_items[i];
            item.name = src_itr->name;
            item.type = src_itr->type;
            item.offset = m_stride;
            item.size = VERTEX_TYPE_BYTES[src_itr->type];
            item.count = VERTEX_TYPE_COUNTS[src_itr->type];

            m_stride += item.size;
        }
    }

    VertexLayout::VertexLayout(const VertexLayout &layout):
        m_items(layout.m_items),
        m_stride(layout.m_stride)
    {}

    VertexLayout::VertexLayout(VertexLayout &&layout):
        m_items(std::move(layout.m_items)),
        m_stride(layout.m_stride)
    {
        layout.m_stride = 0;
    }

    VertexLayout &VertexLayout::operator=(const VertexLayout &layout)
    {
        m_items = layout.m_items;
        m_stride = layout.m_stride;
        return *this;
    }

    VertexLayout &VertexLayout::operator=(VertexLayout &&layout)
    {
        m_items = std::move(layout.m_items);
        m_stride = layout.m_stride;
        layout.m_stride = 0;
        return *this;
    }

    VertexLayout::VecType::const_iterator VertexLayout::begin() const { return m_items.begin(); }
    VertexLayout::VecType::const_iterator VertexLayout::end() const { return m_items.end(); }

    uint32_t VertexLayout::GetStride() const { return m_stride; }
}

#ifdef RUT_HAS_OPENGL
#include"impl/OpenGL/OpenGLMesh.h"

std::shared_ptr<rut::Mesh> rut::Mesh::Create(Context *context, const rut::VertexLayout &layout)
{
    return std::make_shared<rut::impl::OpenGLMesh>(context, layout);
}

std::shared_ptr<rut::Mesh> rut::Mesh::Create(Context *context, rut::VertexLayout &&layout)
{
    return std::make_shared<rut::impl::OpenGLMesh>(context, std::move(layout));
}

#endif