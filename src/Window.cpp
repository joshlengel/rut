#include"RUT/Window.h"
#include"RUT/Config.h"

#ifdef RUT_HAS_X11
#include"impl/X11/X11Window.h"

std::shared_ptr<rut::Window> rut::Window::Create(const WindowProperties &props)
{
    return std::make_shared<rut::impl::X11Window>(props);
}

#endif