#pragma once

#define GL_PIXEL_UNPACK_BUFFER 0x88EC
#define GL_DYNAMIC_DRAW 0x88E8
#define GL_WRITE_ONLY 0x88B9

typedef void (APIENTRY* PFNGLGENBUFFERSARBPROC)(GLsizei n, GLuint* buffers);
typedef void (APIENTRY* PFNGLBINDBUFFERARBPROC)(GLenum target, GLuint buffer);
typedef void (APIENTRY* PFNGLBUFFERDATAARBPROC)(GLenum target, intptr_t size, const void* data, GLenum usage);
typedef void (APIENTRY* PFNGLDELETEBUFFERSARBPROC)(GLsizei n, const GLuint* buffers);
typedef void* (APIENTRY* PFNGLMAPBUFFERARBPROC)(GLenum target, GLenum access);
typedef GLboolean(APIENTRY* PFNGLUNMAPBUFFERARBPROC)(GLenum target);

inline PFNGLGENBUFFERSARBPROC glGenBuffers{ nullptr };
inline PFNGLBINDBUFFERARBPROC glBindBuffer{ nullptr };
inline PFNGLBUFFERDATAARBPROC glBufferData{ nullptr };
inline PFNGLDELETEBUFFERSARBPROC glDeleteBuffers{ nullptr };
inline PFNGLMAPBUFFERARBPROC glMapBuffer{ nullptr };
inline PFNGLUNMAPBUFFERARBPROC glUnmapBuffer{ nullptr };

inline bool InitMiniOpenGL11Func()
{
#pragma warning(push)
#pragma warning(disable: 4191)
	glGenBuffers = reinterpret_cast<PFNGLGENBUFFERSARBPROC>(wglGetProcAddress("glGenBuffers"));
	glBindBuffer = reinterpret_cast<PFNGLBINDBUFFERARBPROC>(wglGetProcAddress("glBindBuffer"));
	glBufferData = reinterpret_cast<PFNGLBUFFERDATAARBPROC>(wglGetProcAddress("glBufferData"));
	glDeleteBuffers = reinterpret_cast<PFNGLDELETEBUFFERSARBPROC>(wglGetProcAddress("glDeleteBuffers"));
	glMapBuffer = reinterpret_cast<PFNGLMAPBUFFERARBPROC>(wglGetProcAddress("glMapBuffer"));
	glUnmapBuffer = reinterpret_cast<PFNGLUNMAPBUFFERARBPROC>(wglGetProcAddress("glUnmapBuffer"));
#pragma warning(pop)

	if (!glGenBuffers || !glBindBuffer || !glBufferData || !glDeleteBuffers || !glMapBuffer || !glUnmapBuffer)
	{
		return false;
	}

	return true;
}