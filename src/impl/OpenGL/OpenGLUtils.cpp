#include"OpenGLUtils.h"

#ifdef RUT_HAS_OPENGL

#define LOAD_FUNC(func) func = reinterpret_cast<decltype(func)>(load_proc(#func))

PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;

PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLBINDBUFFERBASEPROC glBindBufferBase;

PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
PFNGLVERTEXATTRIBIPOINTERPROC glVertexAttribIPointer;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;

PFNGLCREATESHADERPROC glCreateShader;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLDELETEPROGRAMPROC glDeleteProgram;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLDETACHSHADERPROC glDetachShader;
PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
PFNGLBINDFRAGDATALOCATIONPROC glBindFragDataLocation;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLGETACTIVEUNIFORMNAMEPROC glGetActiveUniformName;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLGETUNIFORMBLOCKINDEXPROC glGetUniformBlockIndex;
PFNGLUNIFORMBLOCKBINDINGPROC glUniformBlockBinding;

PFNGLUNIFORM1IPROC glUniform1i;
PFNGLUNIFORM1FPROC glUniform1f;
PFNGLUNIFORM2IPROC glUniform2i;
PFNGLUNIFORM2FPROC glUniform2f;
PFNGLUNIFORM3IPROC glUniform3i;
PFNGLUNIFORM3FPROC glUniform3f;
PFNGLUNIFORM4IPROC glUniform4i;
PFNGLUNIFORM4FPROC glUniform4f;
PFNGLUNIFORM1IVPROC glUniform1iv;
PFNGLUNIFORM1FVPROC glUniform1fv;
PFNGLUNIFORM2IVPROC glUniform2iv;
PFNGLUNIFORM2FVPROC glUniform2fv;
PFNGLUNIFORM3IVPROC glUniform3iv;
PFNGLUNIFORM3FVPROC glUniform3fv;
PFNGLUNIFORM4IVPROC glUniform4iv;
PFNGLUNIFORM4FVPROC glUniform4fv;
PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;

PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;

namespace rut
{
    namespace impl
    {
        void LoadOpenGLFunctions(const std::function<Proc(const char*)> &load_proc)
        {
            // Vertex arrays
            LOAD_FUNC(glGenVertexArrays);
            LOAD_FUNC(glDeleteVertexArrays);
            LOAD_FUNC(glBindVertexArray);

            // Buffers
            LOAD_FUNC(glGenBuffers);
            LOAD_FUNC(glDeleteBuffers);
            LOAD_FUNC(glBindBuffer);
            LOAD_FUNC(glBufferData);
            LOAD_FUNC(glBindBufferBase);

            LOAD_FUNC(glVertexAttribPointer);
            LOAD_FUNC(glVertexAttribIPointer);
            LOAD_FUNC(glEnableVertexAttribArray);

            // Shaders
            LOAD_FUNC(glCreateShader);
            LOAD_FUNC(glDeleteShader);
            LOAD_FUNC(glShaderSource);
            LOAD_FUNC(glCompileShader);
            LOAD_FUNC(glGetShaderiv);
            LOAD_FUNC(glGetShaderInfoLog);

            LOAD_FUNC(glCreateProgram);
            LOAD_FUNC(glDeleteProgram);
            LOAD_FUNC(glUseProgram);
            LOAD_FUNC(glAttachShader);
            LOAD_FUNC(glDetachShader);
            LOAD_FUNC(glBindAttribLocation);
            LOAD_FUNC(glBindFragDataLocation);
            LOAD_FUNC(glLinkProgram);
            LOAD_FUNC(glGetProgramiv);
            LOAD_FUNC(glGetProgramInfoLog);
            LOAD_FUNC(glGetActiveUniformName);
            LOAD_FUNC(glGetUniformLocation);
            LOAD_FUNC(glGetUniformBlockIndex);
            LOAD_FUNC(glUniformBlockBinding);

            LOAD_FUNC(glUniform1i);
            LOAD_FUNC(glUniform1f);
            LOAD_FUNC(glUniform2i);
            LOAD_FUNC(glUniform2f);
            LOAD_FUNC(glUniform3i);
            LOAD_FUNC(glUniform3f);
            LOAD_FUNC(glUniform4i);
            LOAD_FUNC(glUniform4f);
            LOAD_FUNC(glUniform1iv);
            LOAD_FUNC(glUniform1fv);
            LOAD_FUNC(glUniform2iv);
            LOAD_FUNC(glUniform2fv);
            LOAD_FUNC(glUniform3iv);
            LOAD_FUNC(glUniform3fv);
            LOAD_FUNC(glUniform4iv);
            LOAD_FUNC(glUniform4fv);
            LOAD_FUNC(glUniformMatrix3fv);
            LOAD_FUNC(glUniformMatrix4fv);

            // Framebuffers
            LOAD_FUNC(glGenFramebuffers);
            LOAD_FUNC(glDeleteFramebuffers);
            LOAD_FUNC(glBindFramebuffer);
            LOAD_FUNC(glFramebufferTexture2D);
            LOAD_FUNC(glCheckFramebufferStatus);
        }
    }
}

#endif