#include"RUT/Layout.h"

#include<cstring>

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
    LayoutItem::LayoutItem() {}
    LayoutItem::LayoutItem(const LayoutItem &item):
        m_name(item.m_name),
        m_type(item.m_type),
        m_offset(item.m_offset),
        m_size(item.m_size),
        m_count(item.m_count),
        m_length(item.m_length)
    {}

    const std::string &LayoutItem::GetName() const { return m_name; }
    Type LayoutItem::GetType() const { return m_type; }
    uint32_t LayoutItem::GetOffset() const { return m_offset; }
    uint32_t LayoutItem::GetSize() const { return m_size; }
    uint32_t LayoutItem::GetCount() const { return m_count; }
    uint32_t LayoutItem::GetLength() const { return m_length; }
    void LayoutItem::SetName(const std::string &name) { m_name = name; }
    void LayoutItem::SetType(Type type) { m_type = type; }
    void LayoutItem::SetOffset(uint32_t offset) { m_offset = offset; }
    void LayoutItem::SetSize(uint32_t size) { m_size = size; }
    void LayoutItem::SetCount(uint32_t count) { m_count = count; }
    void LayoutItem::SetLength(uint32_t length) { m_length = length; }

    void VertexLayoutItem::Read(void *dst, const void *src) const { std::memcpy(dst, src, GetSize()); }
    void VertexLayoutItem::Write(void *dst, const void *src) const { std::memcpy(dst, src, GetSize()); }

    LayoutItem *VertexLayoutItem::Copy() const { return new VertexLayoutItem(*this); }

    UniformArrayLayoutItem::UniformArrayLayoutItem() {}
    UniformArrayLayoutItem::UniformArrayLayoutItem(const UniformArrayLayoutItem &item):
        LayoutItem(item),
        m_item(item.m_item->Copy()),
        m_stride(item.m_stride)
    {}

    UniformArrayLayoutItem::~UniformArrayLayoutItem()
    {
        delete m_item;
    }

    LayoutItem *UniformArrayLayoutItem::GetItem() const { return m_item; }
    uint32_t UniformArrayLayoutItem::GetStride() const { return m_stride; }
    void UniformArrayLayoutItem::SetItem(LayoutItem *item) { m_item = item; }
    void UniformArrayLayoutItem::SetStride(uint32_t stride) { m_stride = stride; }

    void UniformArrayLayoutItem::Read(void *dst, const void *src) const
    {
        uint8_t *dst_itr = reinterpret_cast<uint8_t*>(dst);
        const uint8_t *src_itr = reinterpret_cast<const uint8_t*>(src);
        for (uint32_t i = 0; i < GetLength(); ++i)
        {
            m_item->Read(dst_itr, src_itr);
            dst_itr += m_item->GetSize();
            src_itr += m_stride;
        }
    }

    void UniformArrayLayoutItem::Write(void *dst, const void *src) const
    {
        uint8_t *dst_itr = reinterpret_cast<uint8_t*>(dst);
        const uint8_t *src_itr = reinterpret_cast<const uint8_t*>(src);
        for (uint32_t i = 0; i < GetLength(); ++i)
        {
            m_item->Read(dst_itr, src_itr);
            dst_itr += m_stride;
            src_itr += m_item->GetSize();
        }
    }

    LayoutItem *UniformArrayLayoutItem::Copy() const { return new UniformArrayLayoutItem(*this); }

    UniformMatrixLayoutItem::UniformMatrixLayoutItem() {}
    UniformMatrixLayoutItem::UniformMatrixLayoutItem(const UniformMatrixLayoutItem &item):
        LayoutItem(item),
        m_stride(item.m_stride),
        m_dimension(item.m_dimension)
    {}

    uint32_t UniformMatrixLayoutItem::GetStride() const { return m_stride; }
    uint32_t UniformMatrixLayoutItem::GetDimension() const { return m_dimension; }
    void UniformMatrixLayoutItem::SetStride(uint32_t stride) { m_stride = stride; }
    void UniformMatrixLayoutItem::SetDimension(uint32_t dimension) { m_dimension = dimension; }

    void UniformMatrixLayoutItem::Read(void *dst, const void *src) const
    {
        uint8_t *dst_itr = reinterpret_cast<uint8_t*>(dst);
        const uint8_t *src_itr = reinterpret_cast<const uint8_t*>(src);
        for (uint32_t i = 0; i < m_dimension; ++i)
        {
            std::memcpy(dst_itr, src_itr, m_dimension * sizeof(float));
            dst_itr += m_dimension * sizeof(float);
            src_itr += m_stride;
        }
    }

    void UniformMatrixLayoutItem::Write(void *dst, const void *src) const
    {
        uint8_t *dst_itr = reinterpret_cast<uint8_t*>(dst);
        const uint8_t *src_itr = reinterpret_cast<const uint8_t*>(src);
        for (uint32_t i = 0; i < m_dimension; ++i)
        {
            std::memcpy(dst_itr, src_itr, m_dimension * sizeof(float));
            dst_itr += m_stride;
            src_itr += m_dimension * sizeof(float);
        }
    }

    LayoutItem *UniformMatrixLayoutItem::Copy() const { return new UniformMatrixLayoutItem(*this); }

    VertexLayout::VertexLayout(std::initializer_list<VertexLayoutElement> il):
        m_items(il.size()),
        m_stride(0)
    {
        auto src_itr = il.begin();
        for (size_t i = 0; i < il.size(); ++i, ++src_itr)
        {
            VertexLayoutItem &item = m_items[i];
            item.SetName(src_itr->name);
            item.SetType(src_itr->type);
            item.SetOffset(m_stride);
            item.SetSize(VERTEX_TYPE_BYTES[src_itr->type]);
            item.SetCount(VERTEX_TYPE_COUNTS[src_itr->type]);

            m_stride += item.GetSize();
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

    UniformLayout::UniformLayout(std::initializer_list<UniformLayoutElement> il):
        m_items(il.size()),
        m_stride(0)
    {
        auto src_itr = il.begin();
        for (size_t i = 0; i < il.size(); ++i, ++src_itr)
        {
            LayoutItem *&item = m_items[i];
            LayoutItem *base_item;
            switch (src_itr->type)
            {
                case VT_MAT3:
                {
                    UniformMatrixLayoutItem *mat_item = new UniformMatrixLayoutItem;
                    mat_item->SetDimension(3);
                    base_item = mat_item;
                    break;
                }

                case VT_MAT4:
                {
                    UniformMatrixLayoutItem *mat_item = new UniformMatrixLayoutItem;
                    mat_item->SetDimension(4);
                    base_item = mat_item;
                    break;
                }

                default:
                {
                    base_item = new VertexLayoutItem;
                    break;
                }
            }

            base_item->SetName(src_itr->name);
            base_item->SetType(src_itr->type);
            base_item->SetCount(VERTEX_TYPE_COUNTS[src_itr->type]);
            base_item->SetLength(1);

            if (src_itr->length > 1)
            {
                UniformArrayLayoutItem *arr_item = new UniformArrayLayoutItem;
                arr_item->SetItem(base_item);
                arr_item->SetLength(src_itr->length);
                base_item = arr_item;
            }

            item = base_item;
        }
    }

    UniformLayout::UniformLayout(const UniformLayout &layout):
        m_items(layout.m_items),
        m_stride(layout.m_stride)
    {}

    UniformLayout::UniformLayout(UniformLayout &&layout):
        m_items(std::move(layout.m_items)),
        m_stride(layout.m_stride)
    {
        layout.m_stride = 0;
    }

    UniformLayout::~UniformLayout()
    {
        for (LayoutItem *item : m_items)
            delete item;
    }

    UniformLayout &UniformLayout::operator=(const UniformLayout &layout)
    {
        for (LayoutItem *item : m_items)
            delete item;
        m_items.clear();

        for (LayoutItem *item : layout.m_items)
            m_items.push_back(item->Copy());

        m_stride = layout.m_stride;
        return *this;
    }

    UniformLayout &UniformLayout::operator=(UniformLayout &&layout)
    {
        m_items = std::move(layout.m_items);
        m_stride = layout.m_stride;
        layout.m_stride = 0;
        return *this;
    }

    UniformLayout::VecType::const_iterator UniformLayout::begin() const { return m_items.begin(); }
    UniformLayout::VecType::const_iterator UniformLayout::end() const { return m_items.end(); }
    UniformLayout::VecType::iterator UniformLayout::begin() { return m_items.begin(); }
    UniformLayout::VecType::iterator UniformLayout::end() { return m_items.end(); }

    uint32_t UniformLayout::GetStride() const { return m_stride; }
    void UniformLayout::SetStride(uint32_t stride) { m_stride = stride; }
}