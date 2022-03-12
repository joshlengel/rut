#pragma once


#include<cstdint>
#include<string>
#include<vector>

namespace rut
{
    enum VertexType
    {
        VT_INT,
        VT_FLOAT,
        VT_IVEC2,
        VT_FVEC2,
        VT_IVEC3,
        VT_FVEC3,
        VT_IVEC4,
        VT_FVEC4,
        VT_MAT3,
        VT_MAT4
    };

    struct VertexLayoutElement
    {
        std::string name;
        VertexType type;
        uint32_t length = 1;
    };

    struct VertexLayoutItem
    {
        std::string name;
        VertexType type;
        uint32_t offset;
        uint32_t size;
        uint32_t count;
    };

    struct VertexLayout
    {
    public:
        typedef std::vector<VertexLayoutItem> VecType;

        VertexLayout(std::initializer_list<VertexLayoutElement> il);
        VertexLayout(const VertexLayout &layout);
        VertexLayout(VertexLayout &&layout);

        VertexLayout &operator=(const VertexLayout &layout);
        VertexLayout &operator=(VertexLayout &&layout);

        VecType::const_iterator begin() const;
        VecType::const_iterator end() const;

        uint32_t GetStride() const;
    
    private:
        VecType m_items;
        uint32_t m_stride;
    };
}