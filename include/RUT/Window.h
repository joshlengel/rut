#pragma once

#include"InputCodes.h"

#include<cstdint>
#include<string>
#include<memory>
#include<functional>

#include<glm/vec2.hpp>

namespace rut
{
    class Event;
    class Context;

    struct WindowProperties
    {
        uint32_t width, height;
        std::string title = "RUT Window";
        bool fullscreen = false;
    };

    class Window
    {
    public:
        typedef std::function<void(Event &e)> EventHandler;

        virtual ~Window() = default;

        virtual const WindowProperties &GetProps() const = 0;

        virtual void Show() = 0;
        virtual void PollEvents() = 0;

        virtual bool ShouldClose() const = 0;
        virtual void SetEventHandler(const EventHandler &handler) = 0;

        virtual bool KeyDown(KeyCode key_code) const = 0;
        virtual bool MouseButtonDown(MouseButtonCode mouse_button_code) const = 0;
        virtual glm::vec2 GetCursorPosition() const = 0;

        virtual Context *GetContext() const = 0;
        virtual uint64_t GetHandle() const = 0;

        static std::shared_ptr<Window> Create(const WindowProperties &props);
    };
};