#include"RUT/Api.h"
#include"RUT/Config.h"

#include<stdexcept>
#include<iostream>

static const std::string WINDOW_API_NAMES[] =
{
	"NONE",
	"X11",
	"WIN32"
};

static const std::string CONTEXT_API_NAMES[] =
{
	"NONE",
	"GLX",
	"EGL",
	"WGL"
};

static const std::string RENDER_API_NAMES[] =
{
	"NONE",
	"OPENGL",
	"VULKAN",
	"DX11"
};

namespace rut
{
    WindowApi Api::s_window_api = WINDOW_API_NONE;
    ContextApi Api::s_context_api = CONTEXT_API_NONE;
    RenderApi Api::s_render_api = RENDER_API_NONE;

    WindowApi Api::GetWindowApi() { return s_window_api; }
    ContextApi Api::GetContextApi() { return s_context_api; }
    RenderApi Api::GetRenderApi() { return s_render_api; }

    void Api::SetWindowApi(WindowApi api) { s_window_api = api; }
    void Api::SetContextApi(ContextApi api) { s_context_api = api; }
    void Api::SetRenderApi(RenderApi api) { s_render_api = api; }

    void Api::ChooseDefaults()
    {
        s_window_api = WINDOW_API_NONE;
        s_context_api = CONTEXT_API_NONE;
        s_render_api = RENDER_API_NONE;

#ifdef RUT_PLATFORM_WINDOWS
        // Pretty straightforward on windows
        s_window_api = WINDOW_API_WIN32;
        s_context_api = CONTEXT_API_WGL;
        s_render_api = RENDER_API_OPENGL;//RENDER_API_DX11;
#elif defined(RUT_PLATFORM_LINUX)
#ifdef RUT_HAS_X11
        s_window_api = WINDOW_API_X11;
#endif
		// Favor EGL over GLX
#ifdef RUT_HAS_OPENGL
        s_render_api = RENDER_API_OPENGL;
#ifdef RUT_HAS_GLX
        s_context_api = CONTEXT_API_GLX;
#endif
#ifdef RUT_HAS_EGL
        s_context_api = CONTEXT_API_EGL;
#endif
#endif
#ifdef RUT_HAS_VULKAN
		s_render_api = RENDER_API_VULKAN;
		s_context_api = CONTEXT_API_KHR_SURFACE;
#endif
#endif
    }

    void Api::CheckCompatibility()
    {
        // Do some basic checks to weed out nonsensical combinations

#ifdef RUT_PLATFORM_WINDOWS
		if (s_window_api == WINDOW_API_X11)
            throw std::runtime_error("Attempting to use WINDOW_API_X11 on windows");
		
		if (s_context_api == CONTEXT_API_GLX)
			throw std::runtime_error("Attempting to use CONTEXT_API_GLX on windows");
#elif defined (RUT_PLATFORM_LINUX)
		if (s_window_api == WINDOW_API_WIN32)
            throw std::runtime_error("Attempting to use WINDOW_API_WIN32 on linux");
		
		if (s_context_api == CONTEXT_API_WGL)
			throw std::runtime_error("Attempting to use CONTEXT_API_WGL on linux");
		
		if (s_render_api == RENDER_API_DX11)
			throw std::runtime_error("Attempting to use RENDER_API_DX11 on linux");
#endif
#ifndef RUT_HAS_VULKAN
		if (s_context_api == CONTEXT_API_KHR_SURFACE)
			throw std::runtime_error("Attempting to use CONTEXT_API_KHR_SURFACE without Vulkan SDK");

		if (s_render_api == RENDER_API_VULKAN)
			throw std::runtime_error("Attempting to use RENDER_API_VULKAN without Vulkan SDK");
#else
		if ((s_render_api == RENDER_API_VULKAN && s_context_api != CONTEXT_API_KHR_SURFACE) || (s_render_api != RENDER_API_VULKAN && s_context_api == CONTEXT_API_KHR_SURFACE))
			throw std::runtime_error("Attempting to use either RENDER_API_VULKAN or CONTEXT_API_KHR_SURFACE but not both");
#endif

		// Check if apis available
#ifndef RUT_HAS_X11
		if (s_window_api == WINDOW_API_X11)
			throw std::runtime_error("Requested WINDOW_API 'X11' not available");
#endif
#ifndef RUT_HAS_WIN32
		if (s_window_api == WINDOW_API_WIN32)
			throw std::runtime_error("Requested WINDOW_API 'WIN32' not available");
#endif
#ifndef RUT_HAS_GLX
		if (s_context_api == CONTEXT_API_GLX)
			throw std::runtime_error("Requested CONTEXT_API 'GLX' not available");
#endif
#ifndef RUT_HAS_EGL
		if (s_context_api == CONTEXT_API_EGL)
			throw std::runtime_error("Requested CONTEXT_API 'EGL' not available");
#endif
#ifndef RUT_HAS_WGL
		if (s_context_api == CONTEXT_API_WGL)
			throw std::runtime_error("Requested CONTEXT_API 'WGL' not available");
#endif
#ifndef RUT_HAS_OPENGL
		if (s_render_api == RENDER_API_OPENGL)
			throw std::runtime_error("Requested RENDER_API 'OPENGL' not available");
#endif
#ifndef RUT_HAS_VULKAN
		if (s_render_api == RENDER_API_VULKAN)
			throw std::runtime_error("Requested RENDER_API 'VULKAN' not available");
#endif
#ifndef RUT_HAS_DX11
		if (s_render_api == RENDER_API_DX11)
			throw std::runtime_error("Requested RENDER_API 'DX11' not available");
#endif
    }

	void Api::PrintInfo()
	{
		std::cout
			<< "----------------- RUT Api info -----------------\n"
			<< "--- Window Api: " << WINDOW_API_NAMES[s_window_api] << "\n"
			<< "--- Context Api: " << CONTEXT_API_NAMES[s_context_api] << "\n"
			<< "--- Render Api: " << RENDER_API_NAMES[s_render_api] << "\n"
			<< "------------------------------------------------" << std::endl;
	}

	void Api::SetupDefault()
	{
		ChooseDefaults();
		CheckCompatibility();
		PrintInfo();
	}
}