#pragma once

#include<cstdint>

#ifdef _WIN32
#define RUT_PLATFORM_WINDOWS
#elif defined(__linux__)
#define RUT_PLATFORM_LINUX
#else
#error Platform unsupported
#endif

#ifdef NDEBUG
#define RUT_BUILD_RELEASE
#else
#define RUT_BUILD_DEBUG
#endif

namespace rut
{
    enum WindowApi : uint8_t
    {
        WINDOW_API_NONE = 0,
        WINDOW_API_X11,
        WINDOW_API_WIN32
    };

    enum ContextApi : uint8_t
    {
        CONTEXT_API_NONE = 0,
        CONTEXT_API_GLX,
        CONTEXT_API_EGL,
        CONTEXT_API_WGL
    };

    enum RenderApi : uint8_t
    {
        RENDER_API_NONE = 0,
        RENDER_API_OPENGL,
        RENDER_API_VULKAN,
        RENDER_API_DX11
    };

    struct Api
    {
    public:
        static WindowApi GetWindowApi();
        static ContextApi GetContextApi();
        static RenderApi GetRenderApi();

        static void SetWindowApi(WindowApi api);
        static void SetContextApi(ContextApi api);
        static void SetRenderApi(RenderApi api);

        static void ChooseDefaults();
        static void CheckCompatibility();

        static void PrintInfo();
        
        static void SetupDefault();

    private:
        Api() {}

        static WindowApi s_window_api;
        static ContextApi s_context_api;
        static RenderApi s_render_api;
    };
}