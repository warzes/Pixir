#pragma once

#define GL_PIXEL_UNPACK_BUFFER 0x88EC
#define GL_DYNAMIC_DRAW 0x88E8
#define GL_WRITE_ONLY ox88B9

typedef void (APIENTRY* PFNGLGENBUFFERSARBPROC)(GLsizei n, GLuint* buffers);
typedef void (APIENTRY* PFNGLBINDBUFFERARBPROC)(GLenum target, GLuint buffer);
typedef void (APIENTRY* PFNGLBUFFERDATAARBPROC)(GLenum target, intptr_t size, const void* data, GLenum usage);
typedef void (APIENTRY* PFNGLDELETEBUFFERSARBPROC)(GLsizei n, const GLuint* buffers);
typedef void* (APIENTRY* PFNGLMAPBUFFERARBPROC)(GLenum target, GLenum access);
typedef GLboolean(APIENTRY* PFNGLUNMAPBUFFERARBPROC)(GLenum target);

PFNGLGENBUFFERSARBPROC glGenBuffers{ nullptr };
PFNGLBINDBUFFERARBPROC glBindBuffer{ nullptr };
PFNGLBUFFERDATAARBPROC glBufferData{ nullptr };
PFNGLDELETEBUFFERSARBPROC glDeleteBuffers{ nullptr };
PFNGLMAPBUFFERARBPROC glMapBuffer{ nullptr };
PFNGLUNMAPBUFFERARBPROC glUnmapBuffer{ nullptr };

bool InitMiniOpenGL11Func()
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