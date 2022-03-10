#pragma once

#include"RUT/Config.h"

#ifdef RUT_HAS_WIN32

#include"RUT/Window.h"
#include"RUT/Api.h"

#include<Windows.h>

namespace rut
{
    class Context;

    namespace impl
    {
        class Win32Window : public Window, public std::enable_shared_from_this<Win32Window>
        {
        public:
            Win32Window(const WindowProperties &props);
            virtual ~Win32Window();

            virtual const WindowProperties &GetProps() const override;

            virtual void Show() override;
            virtual void PollEvents() override;

            virtual bool ShouldClose() const override;
            virtual void SetEventHandler(const EventHandler &handler) override;

            virtual bool KeyDown(KeyCode key_code) const override;
            virtual bool MouseButtonDown(MouseButtonCode mouse_button_code) const override;
            virtual glm::vec2 GetCursorPosition() const override;

            virtual Context *GetContext() const override;
            virtual uint64_t GetHandle() const override;
        
        private:
            WindowProperties m_props;
            HINSTANCE m_instance;
            HWND m_window;
            Context *m_context;
            ContextApi m_context_api;

            bool m_should_close;
            EventHandler m_handler;

            static LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
        };
    }
}

#endif