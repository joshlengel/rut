#pragma once

#include<cstdint>
#include<memory>

namespace rut
{
    class Context
    {
    public:
        virtual ~Context() = default;

        virtual uint32_t GetVersionMajor() const = 0;
        virtual uint32_t GetVersionMinor() const = 0;

        virtual void Begin() = 0;
        virtual void End() = 0;

        virtual uint64_t GetHandle() const = 0;
    };
}