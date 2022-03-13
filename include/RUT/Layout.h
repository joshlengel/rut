#pragma once


#include<cstdint>
#include<string>
#include<vector>

namespace rut
{
    enum Type
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
        Type type;
    };

    struct UniformLayoutElement
    {
        std::string name;
        Type type;
        uint32_t length = 1;
    };

    class LayoutItem
    {
    public:
        LayoutItem();
        LayoutItem(const LayoutItem &item);
        virtual ~LayoutItem() = default;

        const std::string &GetName() const;
        Type GetType() const;
        uint32_t GetOffset() const;
        uint32_t GetSize() const;
        uint32_t GetCount() const;
        uint32_t GetLength() const;

        void SetName(const std::string &name);
        void SetType(Type type);
        void SetOffset(uint32_t offset);
        void SetSize(uint32_t size);
        void SetCount(uint32_t count);
        void SetLength(uint32_t length);

        virtual void Read(void *dst, const void *src) const = 0;
        virtual void Write(void *dst, const void *src) const = 0;

        virtual LayoutItem *Copy() const = 0;
    
    private:
        std::string m_name;
        Type m_type;
        uint32_t m_offset, m_size, m_count, m_length;
    };

    class VertexLayoutItem : public LayoutItem
    {
    public:
        virtual void Read(void *dst, const void *src) const override;
        virtual void Write(void *dst, const void *src) const override;

        virtual LayoutItem *Copy() const override;
    };

    class UniformArrayLayoutItem : public LayoutItem
    {
    public:
        UniformArrayLayoutItem();
        UniformArrayLayoutItem(const UniformArrayLayoutItem &item);
        virtual ~UniformArrayLayoutItem();

        LayoutItem *GetItem() const;
        uint32_t GetStride() const;
        void SetItem(LayoutItem *item);
        void SetStride(uint32_t stride);

        virtual void Read(void *dst, const void *src) const override;
        virtual void Write(void *dst, const void *src) const override;

        virtual LayoutItem *Copy() const override;
    
    private:
        LayoutItem *m_item;
        uint32_t m_stride;
    };

    class UniformMatrixLayoutItem : public LayoutItem
    {
    public:
        UniformMatrixLayoutItem();
        UniformMatrixLayoutItem(const UniformMatrixLayoutItem &item);

        uint32_t GetStride() const;
        uint32_t GetDimension() const;
        void SetStride(uint32_t stride);
        void SetDimension(uint32_t dimension);

        virtual void Read(void *dst, const void *src) const override;
        virtual void Write(void *dst, const void *src) const override;

        virtual LayoutItem *Copy() const override;
    
    private:
        uint32_t m_stride, m_dimension;
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

    struct UniformLayout
    {
    public:
        typedef std::vector<LayoutItem*> VecType;

        UniformLayout(std::initializer_list<UniformLayoutElement> il);
        UniformLayout(const UniformLayout &layout);
        UniformLayout(UniformLayout &&layout);
        ~UniformLayout();

        UniformLayout &operator=(const UniformLayout &layout);
        UniformLayout &operator=(UniformLayout &&layout);

        VecType::const_iterator begin() const;
        VecType::const_iterator end() const;
        VecType::iterator begin();
        VecType::iterator end();

        uint32_t GetStride() const;
        void SetStride(uint32_t stride);
    
    private:
        VecType m_items;
        uint32_t m_stride;
    };
}