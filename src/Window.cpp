#include"RUT/Window.h"
#include"RUT/Config.h"
#include"RUT/Api.h"

#include<stdexcept>

#ifdef RUT_HAS_WIN32
#include"impl/Win32/Win32Window.h"
#endif

#ifdef RUT_HAS_X11
#include"impl/X11/X11Window.h"
#endif

std::shared_ptr<rut::Window> rut::Window::Create(const WindowProperties &props)
{
    switch (Api::GetWindowApi())
    {
    case WINDOW_API_NONE:
        throw std::runtime_error("Error creating window. WINDOW_API_NONE selected");
    
    default:
        throw std::runtime_error("Error creating window. Invalid api selected");

#ifdef RUT_HAS_WIN32
    case WINDOW_API_WIN32:
        return std::make_shared<rut::impl::Win32Window>(props);
#endif

#ifdef RUT_HAS_X11
    case WINDOW_API_X11:
        return std::make_shared<rut::impl::X11Window>(props);
#endif
    }
}