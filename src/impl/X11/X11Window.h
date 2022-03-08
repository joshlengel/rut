#pragma once

#include"RUT/Config.h"

#ifdef RUT_HAS_X11

#include"RUT/Window.h"
#include"RUT/Api.h"

#include<X11/Xlib.h>

namespace rut
{
    namespace impl
    {
        class X11Window : public Window, public std::enable_shared_from_this<X11Window>
        {
        public:
            X11Window(const WindowProperties &props);
            virtual ~X11Window();

            virtual const WindowProperties &GetProps() const override;

            virtual void Show() override;
            virtual void Update() override;

            virtual bool ShouldClose() const override;
            virtual void SetEventHandler(const EventHandler &handler) override;

            virtual bool KeyDown(KeyCode key_code) const override;
            virtual bool MouseButtonDown(MouseButtonCode mouse_button_code) const override;
            virtual glm::vec2 GetCursorPosition() const override;

            virtual Context *GetContext() const override;
            virtual uint64_t GetHandle() const override;
        
        private:
            WindowProperties m_props;
            
            ::Display *m_display;
            int m_screen;
            ::Window m_window;
            Context *m_context;
            ContextApi m_context_api;

            bool m_should_close;
            bool m_keys[KC_LAST];
            bool m_mouse_buttons[MBC_LAST];
            glm::vec2 m_old_cursor_pos, m_cursor_pos;

            EventHandler m_handler;

            void OnEvent(::XEvent &event);
        };
    }
}

#endif