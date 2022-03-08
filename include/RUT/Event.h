#pragma once

#include"InputCodes.h"

#include<memory>

#define BEGIN_DECLARE_EVENT(type, base, enum_val)\
class type : public base\
{\
public:\
    virtual EventType GetType() const override { return enum_val; }\
    static EventType GetStaticType() { return enum_val; }

#define END_DECLARE_EVENT() };

namespace rut
{
    class Window;

    enum EventType
    {
        ET_WINDOW_CLOSED,
        ET_KEY_PRESSED,
        ET_KEY_RELEASED,
        ET_MOUSE_BUTTON_PRESSED,
        ET_MOUSE_BUTTON_RELEASED,
        ET_CURSOR_MOVED
    };

    class Event
    {
    public:
        bool handled = false;

        virtual ~Event() = default;

        virtual EventType GetType() const = 0;
    };

    class EventDispatcher
    {
    public:
        EventDispatcher(Event &event): m_event(event) {}

        template <typename T, typename Func>
        void Dispatch(Func &&func)
        {
            if (T::GetStaticType() == m_event.GetType())
                func(dynamic_cast<T&>(m_event));
        }
    
    private:
        Event &m_event;
    };

    class WindowEvent : public Event
    {
    public:
        const std::shared_ptr<Window> window;

        WindowEvent(std::shared_ptr<Window> window): window(window) {}
    };

    class KeyEvent : public Event
    {
    public:
        const KeyCode key_code;

        KeyEvent(KeyCode key_code): key_code(key_code) {}
    };

    class MouseEvent : public Event
    {
    public:
        const MouseButtonCode mouse_button_code;

        MouseEvent(MouseButtonCode mouse_button_code): mouse_button_code(mouse_button_code) {}
    };

    class CursorEvent : public Event
    {
    public:
        const float x, y;

        CursorEvent(float x, float y): x(x), y(y) {}
    };

    BEGIN_DECLARE_EVENT(WindowClosedEvent, WindowEvent, ET_WINDOW_CLOSED)
        WindowClosedEvent(std::shared_ptr<Window> window): WindowEvent(window) {}
    END_DECLARE_EVENT()

    BEGIN_DECLARE_EVENT(KeyPressedEvent, KeyEvent, ET_KEY_PRESSED)
        KeyPressedEvent(KeyCode key_code): KeyEvent(key_code) {}
    END_DECLARE_EVENT()

    BEGIN_DECLARE_EVENT(KeyReleasedEvent, KeyEvent, ET_KEY_RELEASED)
        KeyReleasedEvent(KeyCode key_code): KeyEvent(key_code) {}
    END_DECLARE_EVENT()

    BEGIN_DECLARE_EVENT(MouseButtonPressedEvent, MouseEvent, ET_MOUSE_BUTTON_PRESSED)
        MouseButtonPressedEvent(MouseButtonCode mouse_button_code): MouseEvent(mouse_button_code) {}
    END_DECLARE_EVENT()

    BEGIN_DECLARE_EVENT(MouseButtonReleasedEvent, MouseEvent, ET_MOUSE_BUTTON_RELEASED)
        MouseButtonReleasedEvent(MouseButtonCode mouse_button_code): MouseEvent(mouse_button_code) {}
    END_DECLARE_EVENT()

    BEGIN_DECLARE_EVENT(CursorMovedEvent, CursorEvent, ET_CURSOR_MOVED)
        const float dx, dy;

        CursorMovedEvent(float x, float y, float dx, float dy): CursorEvent(x, y), dx(dx), dy(dy) {}
    END_DECLARE_EVENT()
}