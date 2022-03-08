#pragma once

#include"RUT/Config.h"
#include"RUT/Api.h"

#ifdef RUT_HAS_OPENGL

#ifdef RUT_PLATFORM_LINUX
#define GL_GLEXT_PROTOTYPES
#include<GL/gl.h>
#include<GL/glext.h>
#endif

#endif